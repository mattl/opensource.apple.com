/* Copyright (c) 2011-2013 Dovecot authors, see the included COPYING file */

#include "lib.h"
#include "ioloop.h"
#include "net.h"
#include "istream.h"
#include "write-full.h"
#include "strescape.h"
#include "time-util.h"
#include "settings-parser.h"
#include "mail-user.h"
#include "mail-storage-private.h"
#include "fts-api.h"
#include "fts-indexer.h"

/* APPLE - async search */
#include "imap-client.h"
#include "imap-commands.h"
#include "imap-search.h"
#include <dlfcn.h>

#define INDEXER_NOTIFY_INTERVAL_SECS 10

#define INDEXER_SOCKET_NAME "indexer"
#define INDEXER_WAIT_MSECS 250
#define INDEXER_HANDSHAKE "VERSION\tindexer\t1\t0\n"

/* APPLE - async search
   Indexing can take a long time, so vanilla dovecot-2.1 unwinds back out to the
   main command loop repeatedly to allow other clients to use this process (when
   client_limit > 1).  Unfortunately it also stalls the whole process for
   INDEXER_WAIT_MSECS every time it finds no input from the indexer, which hurts
   responsiveness for those other clients.

   To increase responsiveness we remove the client's I/O (think fd) from the
   main ioloop and add the indexer's instead.  This way the process waits only
   in select/kqueue/poll/whatever, instead of busy-polling the indexer and
   stalling.  Unfortunately implementing this is pretty ugly since it appears no
   other command suspends input from the client like this.  So we use a really
   bad "reacharound" to suspend and resume client input.
   FIXME:  New in dovecot-2.2 is CLIENT_COMMAND_STATE_WAIT_EXTERNAL.
   Could that replace this reacharound?

   The reacharound needs the symbols cmd_search_more and
   cmd_search_more_callback to determine when this optimization is applicable
   but they are not available in the LDA so use dlsym to look them up. */
static bool (*fts_cmd_search_more)(struct client_command_context *);
static void (*fts_cmd_search_more_callback)(struct client_command_context *);
static bool fts_indexer_async_initialized = FALSE;

struct fts_indexer_context {
	struct mailbox *box;

	struct timeval search_start_time, last_notify;
	unsigned int percentage;
	unsigned int timeout_secs;

	char *path;
	int fd;
	struct istream *input;

	/* APPLE - async search */
	struct mail_search_context *search_ctx;
	struct io *io;
	struct timeout *to;
	int input_result;

	unsigned int notified:1;
	unsigned int failed:1;
};

int fts_indexer_cmd(struct mail_user *user, const char *cmd,
		    const char **path_r)
{
	const char *path;
	int fd;

	path = t_strconcat(user->set->base_dir,
			   "/"INDEXER_SOCKET_NAME, NULL);
	fd = net_connect_unix_with_retries(path, 1000);
	if (fd == -1) {
		i_error("net_connect_unix(%s) failed: %m", path);
		return -1;
	}

	cmd = t_strconcat(INDEXER_HANDSHAKE, cmd, NULL);
	if (write_full(fd, cmd, strlen(cmd)) < 0) {
		i_error("write(%s) failed: %m", path);
		i_close_fd(&fd);
		return -1;
	}
	*path_r = path;
	return fd;
}

static void fts_indexer_notify(struct fts_indexer_context *ctx)
{
	unsigned long long elapsed_msecs, est_total_msecs;
	unsigned int eta_secs;

	if (ioloop_time - ctx->last_notify.tv_sec < INDEXER_NOTIFY_INTERVAL_SECS)
		return;
	ctx->last_notify = ioloop_timeval;

	if (ctx->box->storage->callbacks.notify_ok == NULL ||
	    ctx->percentage == 0)
		return;

	elapsed_msecs = timeval_diff_msecs(&ioloop_timeval,
					   &ctx->search_start_time);
	est_total_msecs = elapsed_msecs * 100 / ctx->percentage;
	eta_secs = (est_total_msecs - elapsed_msecs) / 1000;

	T_BEGIN {
		const char *text;

		text = t_strdup_printf("Indexed %d%% of the mailbox, "
				       "ETA %d:%02d", ctx->percentage,
				       eta_secs/60, eta_secs%60);
		ctx->box->storage->callbacks.
			notify_ok(ctx->box, text,
				  ctx->box->storage->callback_context);
		ctx->notified = TRUE;
	} T_END;
}

/* APPLE - async search */
static void fts_indexer_async_init(void)
{
	if (fts_indexer_async_initialized)
		return;

	fts_cmd_search_more = dlsym(RTLD_MAIN_ONLY, "cmd_search_more");
	fts_cmd_search_more_callback = dlsym(RTLD_MAIN_ONLY,
					     "cmd_search_more_callback");
	if (fts_cmd_search_more != NULL && fts_cmd_search_more_callback == NULL)
		i_fatal("fts: found cmd_search_more but not cmd_search_more_callback");

	fts_indexer_async_initialized = TRUE;
}

int fts_indexer_init(struct fts_backend *backend, struct mailbox *box,
		     struct mail_search_context *search_ctx,	/* APPLE - async search */
		     struct fts_indexer_context **ctx_r)
{
	struct fts_indexer_context *ctx;
	struct mailbox_status status;
	uint32_t last_uid, seq1, seq2;
	const char *path, *cmd, *value, *error;
	int fd;

	if (fts_backend_get_last_uid(backend, box, &last_uid) < 0)
		return -1;

	mailbox_get_open_status(box, STATUS_UIDNEXT, &status);
	if (status.uidnext == last_uid+1) {
		/* everything is already indexed */
		return 0;
	}

	mailbox_get_seq_range(box, last_uid+1, (uint32_t)-1, &seq1, &seq2);
	if (seq1 == 0) {
		/* no new messages (last messages in mailbox were expunged) */
		return 0;
	}

	cmd = t_strdup_printf("PREPEND\t1\t%s\t%s\n",
			      str_tabescape(box->storage->user->username),
			      str_tabescape(box->vname));
	fd = fts_indexer_cmd(box->storage->user, cmd, &path);
	if (fd == -1)
		return -1;

	/* connect to indexer and request immediate indexing of the mailbox */
	ctx = i_new(struct fts_indexer_context, 1);
	ctx->box = box;
	ctx->path = i_strdup(path);
	ctx->fd = fd;
	ctx->input = i_stream_create_fd(fd, 128, FALSE);
	ctx->search_start_time = ioloop_timeval;
	ctx->search_ctx = search_ctx;		/* APPLE - async search */

	fts_indexer_async_init();		/* APPLE - async search */

	value = mail_user_plugin_getenv(box->storage->user, "fts_index_timeout");
	if (value != NULL) {
		if (settings_get_time(value, &ctx->timeout_secs, &error) < 0)
			i_error("Invalid fts_index_timeout setting: %s", error);
	}


	*ctx_r = ctx;
	return 1;
}

int fts_indexer_deinit(struct fts_indexer_context **_ctx)
{
	struct fts_indexer_context *ctx = *_ctx;
	int ret = ctx->failed ? -1 : 0;

	*_ctx = NULL;

	i_stream_destroy(&ctx->input);
	if (close(ctx->fd) < 0)
		i_error("close(%s) failed: %m", ctx->path);
	if (ctx->notified) {
		/* we notified at least once */
		ctx->box->storage->callbacks.
			notify_ok(ctx->box, "Mailbox indexing finished",
				  ctx->box->storage->callback_context);
	}
	i_free(ctx->path);
	i_free(ctx);
	return ret;
}

static int fts_indexer_input(struct fts_indexer_context *ctx)
{
	const char *line;
	int percentage;

	while ((line = i_stream_read_next_line(ctx->input)) != NULL) {
		/* initial reply: <tag> \t OK
		   following: <tag> \t <percentage> */
		if (strncmp(line, "1\t", 2) != 0) {
			i_error("indexer sent invalid reply: %s", line);
			return -1;
		}
		line += 2;
		if (strcmp(line, "OK") == 0)
			continue;
		if (str_to_int(line, &percentage) < 0 || percentage > 100) {
			i_error("indexer sent invalid percentage: %s", line);
			return -1;
		}
		if (percentage < 0) {
			/* indexing failed */
			i_error("indexer failed to index mailbox %s",
				ctx->box->vname);
			return -1;
		}
		ctx->percentage = percentage;
		if (percentage == 100) {
			/* finished */
			return 1;
		}
	}
	if (ctx->input->stream_errno != 0) {
		i_error("indexer read() failed: %m");
		return -1;
	}
	if (ctx->input->eof) {
		i_error("indexer disconnected unexpectedly");
		return -1;
	}
	return 0;
}

/* APPLE - async search */
static bool fts_indexer_cancel(struct client_command_context *cmd)
{
	i_assert(cmd->cancel);

	i_assert(cmd->func == fts_indexer_cancel);
	cmd->func = fts_cmd_search_more;

	/* cleanly cancel the search */
	(void) fts_cmd_search_more(cmd);

	cmd->client->ignore = FALSE;

	return TRUE;
}

/* APPLE - async search */
static void fts_indexer_async_stop(struct fts_indexer_context *ctx);
static void fts_indexer_async_input(struct fts_indexer_context *ctx)
{
	int ret;

	if ((ret = fts_indexer_input(ctx)) != 0) {
		ctx->input_result = ret;
		fts_indexer_async_stop(ctx);
	}
}

/* APPLE - async search */
static void fts_indexer_async_timeout(struct fts_indexer_context *ctx)
{
	ctx->failed = TRUE;
	fts_indexer_async_stop(ctx);
}

/* APPLE - async search */
static int fts_indexer_async(struct fts_indexer_context *ctx)
{
	struct imap_search_context *imap_ctx = NULL;
	struct client_command_context *cmd = NULL;

	if (ctx->search_ctx != NULL)
		imap_ctx = ctx->search_ctx->imap_ctx;
	if (imap_ctx != NULL)
		cmd = imap_ctx->cmd;
	if (cmd == NULL || fts_cmd_search_more == NULL ||
	    cmd->func != fts_cmd_search_more)
		return -1;

	if (cmd->client->io != NULL)
		io_remove(&cmd->client->io);
	cmd->client->ignore = TRUE;
	if (imap_ctx->to != NULL)
		timeout_remove(&imap_ctx->to);

	i_assert(ctx->io == NULL);
	ctx->io = io_add(ctx->fd, IO_READ, fts_indexer_async_input, ctx);
	i_assert(ctx->to == NULL);
	if (ctx->timeout_secs > 0)
		ctx->to = timeout_add(ctx->timeout_secs * 1000,
				      fts_indexer_async_timeout, ctx);

	cmd->func = fts_indexer_cancel;
	i_assert(cmd->context != NULL);

	return 0;
}

static int fts_indexer_more_int(struct fts_indexer_context *ctx)
{
	struct ioloop *ioloop;
	struct io *io;
	struct timeout *to;
	int ret;

	/* APPLE - async search */
	if ((ret = ctx->input_result) != 0) {
		ctx->input_result = 0;
		return ret;
	}

	if ((ret = fts_indexer_input(ctx)) != 0)
		return ret;

	/* APPLE - async search */
	if (fts_indexer_async(ctx) == 0)
		return 0;

	/* wait for a while for the reply. FIXME: once search API supports
	   asynchronous waits, get rid of this wait and use the mail IO loop */
	ioloop = io_loop_create();
	io = io_add(ctx->fd, IO_READ, io_loop_stop, ioloop);
	to = timeout_add_short(INDEXER_WAIT_MSECS, io_loop_stop, ioloop);
	io_loop_run(ioloop);
	io_remove(&io);
	timeout_remove(&to);
	io_loop_destroy(&ioloop);

	return fts_indexer_input(ctx);
}

int fts_indexer_more(struct fts_indexer_context *ctx)
{
	int ret, diff;

	if ((ret = fts_indexer_more_int(ctx)) < 0) {
		mail_storage_set_internal_error(ctx->box->storage);
		ctx->failed = TRUE;
		return -1;
	}

	if (ctx->timeout_secs > 0) {
		diff = ioloop_time - ctx->search_start_time.tv_sec;
		if (diff > (int)ctx->timeout_secs) {
			mail_storage_set_error(ctx->box->storage,
				MAIL_ERROR_INUSE,
				"Timeout while waiting for indexing to finish");
			ctx->failed = TRUE;
			return -1;
		}
	}
	if (ret == 0)
		fts_indexer_notify(ctx);
	return ret;
}

/* APPLE - async search - this is down at the end of this file because
   of the #undef, which is necessary because the type-compatibility
   check doesn't like function pointers */
static void fts_indexer_async_stop(struct fts_indexer_context *ctx)
{
	struct imap_search_context *imap_ctx;
	struct client_command_context *cmd;

	/* nobody said reacharounds were pretty.
	   the assertions are for figuring out where it all goes south */
	i_assert(ctx->search_ctx != NULL);
	imap_ctx = ctx->search_ctx->imap_ctx;
	i_assert(imap_ctx != NULL);
	i_assert(imap_ctx->to == NULL);
	cmd = imap_ctx->cmd;
	i_assert(cmd != NULL);
#undef timeout_add_short
	imap_ctx->to = timeout_add_short(0, __LINE__, (io_callback_t *) fts_cmd_search_more_callback, cmd);
	i_assert(cmd->func == fts_indexer_cancel);
	cmd->func = fts_cmd_search_more;

	/* client_add_missing_io() restores cmd->client->io for us */
	cmd->client->ignore = FALSE;
	if (ctx->io != NULL)
		io_remove(&ctx->io);
	if (ctx->to != NULL)
		timeout_remove(&ctx->to);
}
