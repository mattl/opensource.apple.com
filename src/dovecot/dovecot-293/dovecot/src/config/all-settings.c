#include "lib.h"
#include "array.h"
#include "ipwd.h"
#include "var-expand.h"
#include "file-lock.h"
#include "fsync-mode.h"
#include "hash-format.h"
#include "net.h"
#include "unichar.h"
#include "settings-parser.h"
#include "all-settings.h"
#include <stddef.h>
#include <unistd.h>
#define CONFIG_BINARY
extern buffer_t config_all_services_buf;/* ../../src/lib-storage/mail-storage-settings.h */
extern const struct setting_parser_info mail_user_setting_parser_info;
extern const struct setting_parser_info mail_namespace_setting_parser_info;
extern const struct setting_parser_info mail_storage_setting_parser_info;
/* <settings checks> */
#define MAILBOX_SET_AUTO_NO "no"
#define MAILBOX_SET_AUTO_CREATE "create"
#define MAILBOX_SET_AUTO_SUBSCRIBE "subscribe"
/* </settings checks> */
struct mail_storage_settings {
	const char *mail_location;
	const char *mail_attachment_fs;
	const char *mail_attachment_dir;
	const char *mail_attachment_hash;
	uoff_t mail_attachment_min_size;
	const char *mail_attribute_dict;
	unsigned int mail_prefetch_count;
	const char *mail_cache_fields;
	const char *mail_always_cache_fields;
	const char *mail_never_cache_fields;
	unsigned int mail_cache_min_mail_count;
	unsigned int mailbox_idle_check_interval;
	unsigned int mail_max_keyword_length;
	unsigned int mail_max_lock_timeout;
	unsigned int mail_temp_scan_interval;
	bool mail_save_crlf;
	const char *mail_fsync;
	bool mmap_disable;
	bool dotlock_use_excl;
	bool mail_nfs_storage;
	bool mail_nfs_index;
	bool mailbox_list_index;
	bool mail_debug;
	bool mail_full_filesystem_access;
	bool maildir_stat_dirs;
	bool mail_shared_explicit_inbox;
	const char *lock_method;
	const char *pop3_uidl_format;

	const char *ssl_client_ca_dir;
	const char *ssl_client_ca_file;
	const char *ssl_crypto_device;

	enum file_lock_method parsed_lock_method;
	enum fsync_mode parsed_fsync_mode;
};
struct mail_namespace_settings {
	const char *name;
	const char *type;
	const char *separator;
	const char *prefix;
	const char *location;
	const char *alias_for;

	bool inbox;
	bool hidden;
	const char *list;
	bool subscriptions;
	bool ignore_on_failure;
	bool disabled;

	ARRAY(struct mailbox_settings *) mailboxes;
	struct mail_user_settings *user_set;
};
struct mailbox_settings {
	const char *name;
	const char *autocreate;
	const char *special_use;
	const char *driver;
};
struct mail_user_settings {
	const char *base_dir;
	const char *auth_socket_path;
	const char *mail_temp_dir;

	const char *mail_uid;
	const char *mail_gid;
	const char *mail_home;
	const char *mail_chroot;
	const char *mail_access_groups;
	const char *mail_privileged_group;
	const char *valid_chroot_dirs;

	unsigned int first_valid_uid, last_valid_uid;
	unsigned int first_valid_gid, last_valid_gid;

	const char *mail_plugins;
	const char *mail_plugin_dir;

	const char *mail_log_prefix;

	ARRAY(struct mail_namespace_settings *) namespaces;
	ARRAY(const char *) plugin_envs;
};
/* ../../src/lib-storage/index/pop3c/pop3c-settings.h */
struct pop3c_settings {
	const char *pop3c_host;
	unsigned int pop3c_port;

	const char *pop3c_user;
	const char *pop3c_master_user;
	const char *pop3c_password;

	const char *pop3c_ssl;
	bool pop3c_ssl_verify;

	const char *pop3c_rawlog_dir;
};
/* ../../src/lib-storage/index/mbox/mbox-settings.h */
struct mbox_settings {
	const char *mbox_read_locks;
	const char *mbox_write_locks;
	unsigned int mbox_lock_timeout;
	unsigned int mbox_dotlock_change_timeout;
	uoff_t mbox_min_index_size;
	bool mbox_dirty_syncs;
	bool mbox_very_dirty_syncs;
	bool mbox_lazy_writes;
	const char *mbox_md5;
};
/* ../../src/lib-storage/index/maildir/maildir-settings.h */
struct maildir_settings {
	bool maildir_copy_with_hardlinks;
	bool maildir_very_dirty_syncs;
	bool maildir_broken_filename_sizes;
};
/* ../../src/lib-storage/index/imapc/imapc-settings.h */
/* <settings checks> */
enum imapc_features {
	IMAPC_FEATURE_RFC822_SIZE	= 0x01,
	IMAPC_FEATURE_GUID_FORCED	= 0x02
};
/* </settings checks> */
struct imapc_settings {
	const char *imapc_host;
	unsigned int imapc_port;

	const char *imapc_user;
	const char *imapc_master_user;
	const char *imapc_password;

	const char *imapc_ssl;
	bool imapc_ssl_verify;

	const char *imapc_features;
	const char *imapc_rawlog_dir;
	const char *imapc_list_prefix;
	unsigned int imapc_max_idle_time;

	enum imapc_features parsed_features;
};
/* ../../src/lib-storage/index/dbox-multi/mdbox-settings.h */
struct mdbox_settings {
	bool mdbox_preallocate_space;
	uoff_t mdbox_rotate_size;
	unsigned int mdbox_rotate_interval;
};
/* ../../src/lib-settings/settings.h */
#define DEF_STRUCT_STR(name, struct_name) \
	{ SET_STR + COMPILE_ERROR_IF_TYPES_NOT_COMPATIBLE( \
		((struct struct_name *)0)->name, const char *), \
	  #name, offsetof(struct struct_name, name) }
#define DEF_STRUCT_INT(name, struct_name) \
	{ SET_INT + COMPILE_ERROR_IF_TYPES_NOT_COMPATIBLE( \
		((struct struct_name *)0)->name, unsigned int), \
	  #name, offsetof(struct struct_name, name) }
#define DEF_STRUCT_BOOL(name, struct_name) \
	{ SET_BOOL + COMPILE_ERROR_IF_TYPES_NOT_COMPATIBLE( \
		((struct struct_name *)0)->name, bool), \
	  #name, offsetof(struct struct_name, name) }
/* ../../src/lib-master/service-settings.h */
/* <settings checks> */
enum service_user_default {
	SERVICE_USER_DEFAULT_NONE = 0,
	SERVICE_USER_DEFAULT_INTERNAL,
	SERVICE_USER_DEFAULT_LOGIN
};

enum service_type {
	SERVICE_TYPE_UNKNOWN,
	SERVICE_TYPE_LOG,
	SERVICE_TYPE_ANVIL,
	SERVICE_TYPE_CONFIG,
	SERVICE_TYPE_LOGIN,
	SERVICE_TYPE_STARTUP
};
/* </settings checks> */
struct file_listener_settings {
	const char *path;
	unsigned int mode;
	const char *user;
	const char *group;
};
ARRAY_DEFINE_TYPE(file_listener_settings, struct file_listener_settings *);
struct inet_listener_settings {
	const char *name;
	const char *address;
	unsigned int port;
	bool ssl;
};
ARRAY_DEFINE_TYPE(inet_listener_settings, struct inet_listener_settings *);
struct service_settings {
	const char *name;
	const char *protocol;
	const char *type;
	const char *executable;
	const char *user;
	const char *group;
	const char *privileged_group;
	const char *extra_groups;
	const char *chroot;

	bool drop_priv_before_exec;

	unsigned int process_min_avail;
	unsigned int process_limit;
	unsigned int client_limit;
	unsigned int service_count;
	unsigned int idle_kill;
	uoff_t vsz_limit;

	ARRAY_TYPE(file_listener_settings) unix_listeners;
	ARRAY_TYPE(file_listener_settings) fifo_listeners;
	ARRAY_TYPE(inet_listener_settings) inet_listeners;

	/* internal to master: */
	struct master_settings *master_set;
	enum service_type parsed_type;
	enum service_user_default user_default;
	unsigned int login_dump_core:1;

	/* -- flags that can be set internally -- */

	/* process_limit must not be higher than 1 */
	unsigned int process_limit_1:1;
};
ARRAY_DEFINE_TYPE(service_settings, struct service_settings *);
/* ../../src/lib-master/master-service-ssl-settings.h */
extern const struct setting_parser_info master_service_ssl_setting_parser_info;
struct master_service_ssl_settings {
	const char *ssl;
	const char *ssl_ca;
	const char *ssl_cert;
	const char *ssl_key;
#ifdef APPLE_OS_X_SERVER
	const char *ssl_key_path;
#endif
	const char *ssl_key_password;
	const char *ssl_cipher_list;
	const char *ssl_protocols;
	const char *ssl_cert_username_field;
	const char *ssl_crypto_device;
	bool ssl_verify_client_cert;
	bool ssl_require_crl;
	bool verbose_ssl;
};
/* ../../src/lib-master/master-service-settings.h */
extern const struct setting_parser_info master_service_setting_parser_info;
struct master_service_settings {
	const char *base_dir;
	const char *state_dir;
	const char *log_path;
	const char *info_log_path;
	const char *debug_log_path;
	const char *log_timestamp;
	const char *syslog_facility;
	uoff_t config_cache_size;
	bool version_ignore;
	bool shutdown_clients;
	bool verbose_proctitle;
};
/* ../../src/lib-lda/lda-settings.h */
extern const struct setting_parser_info lda_setting_parser_info;
struct lda_settings {
	const char *postmaster_address;
	const char *hostname;
	const char *submission_host;
	const char *sendmail_path;
	const char *rejection_subject;
	const char *rejection_reason;
	const char *deliver_log_format;
	const char *recipient_delimiter;
	const char *lda_original_recipient_header;

	bool quota_full_tempfail;
	bool lda_mailbox_autocreate;
	bool lda_mailbox_listid_autosave;	/* Apple */
	bool lda_mailbox_autosubscribe;
};
/* ../../src/lib-dict/dict-sql-settings.h */
struct dict_sql_settings {
	const char *connect;

	unsigned int max_field_count;
	ARRAY(struct dict_sql_map) maps;
};
/* ../../src/lib-storage/mail-storage-settings.c */
extern const struct setting_parser_info mailbox_setting_parser_info;
extern const struct setting_parser_info mail_namespace_setting_parser_info;
/* <settings checks> */
static bool mail_storage_settings_check(void *_set, pool_t pool ATTR_UNUSED,
					const char **error_r)
{
	struct mail_storage_settings *set = _set;
	struct hash_format *format;
	const char *p, *error;
	bool uidl_format_ok;
	char c;

	if (set->mailbox_idle_check_interval == 0) {
		*error_r = "mailbox_idle_check_interval must not be 0";
		return FALSE;
	}

	if (strcmp(set->mail_fsync, "optimized") == 0)
		set->parsed_fsync_mode = FSYNC_MODE_OPTIMIZED;
	else if (strcmp(set->mail_fsync, "never") == 0)
		set->parsed_fsync_mode = FSYNC_MODE_NEVER;
	else if (strcmp(set->mail_fsync, "always") == 0)
		set->parsed_fsync_mode = FSYNC_MODE_ALWAYS;
	else {
		*error_r = t_strdup_printf("Unknown mail_fsync: %s",
					   set->mail_fsync);
		return FALSE;
	}

	if (set->mail_nfs_index && !set->mmap_disable) {
		*error_r = "mail_nfs_index=yes requires mmap_disable=yes";
		return FALSE;
	}
	if (set->mail_nfs_index &&
	    set->parsed_fsync_mode != FSYNC_MODE_ALWAYS) {
		*error_r = "mail_nfs_index=yes requires mail_fsync=always";
		return FALSE;
	}

	if (!file_lock_method_parse(set->lock_method,
				    &set->parsed_lock_method)) {
		*error_r = t_strdup_printf("Unknown lock_method: %s",
					   set->lock_method);
		return FALSE;
	}

	uidl_format_ok = FALSE;
	for (p = set->pop3_uidl_format; *p != '\0'; p++) {
		if (p[0] != '%' || p[1] == '\0')
			continue;

		c = var_get_key(++p);
		switch (c) {
		case 'v':
		case 'u':
		case 'm':
		case 'f':
		case 'g':
			uidl_format_ok = TRUE;
			break;
		case '%':
			break;
		default:
			*error_r = t_strdup_printf(
				"Unknown pop3_uidl_format variable: %%%c", c);
			return FALSE;
		}
	}
	if (!uidl_format_ok) {
		*error_r = "pop3_uidl_format setting doesn't contain any "
			"%% variables.";
		return FALSE;
	}

	if (strchr(set->mail_attachment_hash, '/') != NULL) {
		*error_r = "mail_attachment_hash setting "
			"must not contain '/' characters";
		return FALSE;
	}
	if (hash_format_init(set->mail_attachment_hash, &format, &error) < 0) {
		*error_r = t_strconcat("Invalid mail_attachment_hash setting: ",
				       error, NULL);
		return FALSE;
	}
	if (strchr(set->mail_attachment_hash, '-') != NULL) {
		*error_r = "mail_attachment_hash setting "
			"must not contain '-' characters";
		return FALSE;
	}
	hash_format_deinit_free(&format);
#ifndef CONFIG_BINARY
	if (*set->ssl_client_ca_dir != '\0' &&
	    access(set->ssl_client_ca_dir, X_OK) < 0) {
		*error_r = t_strdup_printf(
			"ssl_client_ca_dir: access(%s) failed: %m",
			set->ssl_client_ca_dir);
		return FALSE;
	}
#endif
	return TRUE;
}

static bool namespace_settings_check(void *_set, pool_t pool ATTR_UNUSED,
				     const char **error_r)
{
	struct mail_namespace_settings *ns = _set;
	struct mail_namespace_settings *const *namespaces;
	const char *name;
	unsigned int i, count;

	name = ns->prefix != NULL ? ns->prefix : "";

	if (ns->separator[0] != '\0' && ns->separator[1] != '\0') {
		*error_r = t_strdup_printf("Namespace '%s': "
			"Hierarchy separator must be only one character long",
			name);
		return FALSE;
	}
	if (!uni_utf8_str_is_valid(name)) {
		*error_r = t_strdup_printf("Namespace prefix not valid UTF8: %s",
					   name);
		return FALSE;
	}

	if (ns->alias_for != NULL && !ns->disabled) {
		if (array_is_created(&ns->user_set->namespaces)) {
			namespaces = array_get(&ns->user_set->namespaces,
					       &count);
		} else {
			namespaces = NULL;
			count = 0;
		}
		for (i = 0; i < count; i++) {
			if (strcmp(namespaces[i]->prefix, ns->alias_for) == 0)
				break;
		}
		if (i == count) {
			*error_r = t_strdup_printf(
				"Namespace '%s': alias_for points to "
				"unknown namespace: %s", name, ns->alias_for);
			return FALSE;
		}
		if (namespaces[i]->alias_for != NULL) {
			*error_r = t_strdup_printf(
				"Namespace '%s': alias_for chaining isn't "
				"allowed: %s -> %s", name, ns->alias_for,
				namespaces[i]->alias_for);
			return FALSE;
		}
	}
	return TRUE;
}

static bool mailbox_special_use_exists(const char *name)
{
	if (name[0] != '\\')
		return FALSE;
	name++;

	if (strcasecmp(name, "All") == 0)
		return TRUE;
	if (strcasecmp(name, "Archive") == 0)
		return TRUE;
	if (strcasecmp(name, "Drafts") == 0)
		return TRUE;
	if (strcasecmp(name, "Flagged") == 0)
		return TRUE;
	if (strcasecmp(name, "Junk") == 0)
		return TRUE;
	if (strcasecmp(name, "Sent") == 0)
		return TRUE;
	if (strcasecmp(name, "Trash") == 0)
		return TRUE;
	return FALSE;
}

static bool
mailbox_special_use_check(struct mailbox_settings *set, pool_t pool,
			  const char **error_r)
{
	const char *const *uses, *str;
	unsigned int i;

	uses = t_strsplit_spaces(set->special_use, " ");
	for (i = 0; uses[i] != NULL; i++) {
		if (!mailbox_special_use_exists(uses[i])) {
			*error_r = t_strdup_printf(
				"mailbox %s: unknown special_use: %s",
				set->name, uses[i]);
			return FALSE;
		}
	}
	/* make sure there are no extra spaces */
	str = t_strarray_join(uses, " ");
	if (strcmp(str, set->special_use) != 0)
		set->special_use = p_strdup(pool, str);
	return TRUE;
}

static bool mailbox_settings_check(void *_set, pool_t pool,
				   const char **error_r)
{
	struct mailbox_settings *set = _set;

	if (!uni_utf8_str_is_valid(set->name)) {
		*error_r = t_strdup_printf("mailbox %s: name isn't valid UTF-8",
					   set->name);
		return FALSE;
	}
	if (*set->special_use != '\0') {
		if (!mailbox_special_use_check(set, pool, error_r))
			return FALSE;
	}
	return TRUE;
}

static bool mail_user_settings_check(void *_set, pool_t pool ATTR_UNUSED,
				     const char **error_r ATTR_UNUSED)
{
	struct mail_user_settings *set = _set;

#ifndef CONFIG_BINARY
	fix_base_path(set, pool, &set->auth_socket_path);
#else
	if (*set->mail_plugins != '\0' &&
	    access(set->mail_plugin_dir, R_OK | X_OK) < 0) {
		*error_r = t_strdup_printf(
			"mail_plugin_dir: access(%s) failed: %m",
			set->mail_plugin_dir);
		return FALSE;
	}
#endif
	return TRUE;
}
/* </settings checks> */
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct mail_storage_settings, name), NULL }
static const struct setting_define mail_storage_setting_defines[] = {
	DEF(SET_STR_VARS, mail_location),
	{ SET_ALIAS, "mail", 0, NULL },
	DEF(SET_STR_VARS, mail_attachment_fs),
	DEF(SET_STR_VARS, mail_attachment_dir),
	DEF(SET_STR, mail_attachment_hash),
	DEF(SET_SIZE, mail_attachment_min_size),
	DEF(SET_STR_VARS, mail_attribute_dict),
	DEF(SET_UINT, mail_prefetch_count),
	DEF(SET_STR, mail_cache_fields),
	DEF(SET_STR, mail_always_cache_fields),
	DEF(SET_STR, mail_never_cache_fields),
	DEF(SET_UINT, mail_cache_min_mail_count),
	DEF(SET_TIME, mailbox_idle_check_interval),
	DEF(SET_UINT, mail_max_keyword_length),
	DEF(SET_TIME, mail_max_lock_timeout),
	DEF(SET_TIME, mail_temp_scan_interval),
	DEF(SET_BOOL, mail_save_crlf),
	DEF(SET_ENUM, mail_fsync),
	DEF(SET_BOOL, mmap_disable),
	DEF(SET_BOOL, dotlock_use_excl),
	DEF(SET_BOOL, mail_nfs_storage),
	DEF(SET_BOOL, mail_nfs_index),
	DEF(SET_BOOL, mailbox_list_index),
	DEF(SET_BOOL, mail_debug),
	DEF(SET_BOOL, mail_full_filesystem_access),
	DEF(SET_BOOL, maildir_stat_dirs),
	DEF(SET_BOOL, mail_shared_explicit_inbox),
	DEF(SET_ENUM, lock_method),
	DEF(SET_STR, pop3_uidl_format),

	DEF(SET_STR, ssl_client_ca_dir),
	DEF(SET_STR, ssl_client_ca_file),
	DEF(SET_STR, ssl_crypto_device),

	SETTING_DEFINE_LIST_END
};
const struct mail_storage_settings mail_storage_default_settings = {
	.mail_location = "",
	.mail_attachment_fs = "sis posix",
	.mail_attachment_dir = "",
	.mail_attachment_hash = "%{sha1}",
	.mail_attachment_min_size = 1024*128,
	.mail_attribute_dict = "",
	.mail_prefetch_count = 0,
	.mail_cache_fields = "flags",
	.mail_always_cache_fields = "",
	.mail_never_cache_fields = "imap.envelope",
	.mail_cache_min_mail_count = 0,
	.mailbox_idle_check_interval = 30,
	.mail_max_keyword_length = 50,
	.mail_max_lock_timeout = 0,
	.mail_temp_scan_interval = 7*24*60*60,
	.mail_save_crlf = FALSE,
	.mail_fsync = "optimized:never:always",
	.mmap_disable = FALSE,
	.dotlock_use_excl = TRUE,
	.mail_nfs_storage = FALSE,
	.mail_nfs_index = FALSE,
	.mailbox_list_index = FALSE,
	.mail_debug = FALSE,
	.mail_full_filesystem_access = FALSE,
	.maildir_stat_dirs = FALSE,
	.mail_shared_explicit_inbox = FALSE,
	.lock_method = "fcntl:flock:dotlock",
	.pop3_uidl_format = "%08Xu%08Xv",

	.ssl_client_ca_dir = "",
	.ssl_client_ca_file = "",
	.ssl_crypto_device = ""
};
const struct setting_parser_info mail_storage_setting_parser_info = {
	.module_name = "mail",
	.defines = mail_storage_setting_defines,
	.defaults = &mail_storage_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct mail_storage_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info,

	.check_func = mail_storage_settings_check
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct mailbox_settings, name), NULL }
static const struct setting_define mailbox_setting_defines[] = {
	DEF(SET_STR, name),
	{ SET_ENUM, "auto", offsetof(struct mailbox_settings, autocreate), NULL } ,
	DEF(SET_STR, special_use),
	DEF(SET_STR, driver),

	SETTING_DEFINE_LIST_END
};
const struct mailbox_settings mailbox_default_settings = {
	.name = "",
	.autocreate = MAILBOX_SET_AUTO_NO":"
		MAILBOX_SET_AUTO_CREATE":"
		MAILBOX_SET_AUTO_SUBSCRIBE,
	.special_use = "",
	.driver = ""
};
const struct setting_parser_info mailbox_setting_parser_info = {
	.defines = mailbox_setting_defines,
	.defaults = &mailbox_default_settings,

	.type_offset = offsetof(struct mailbox_settings, name),
	.struct_size = sizeof(struct mailbox_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info,

	.check_func = mailbox_settings_check
};
#undef DEF
#undef DEFLIST_UNIQUE
#define DEF(type, name) \
	{ type, #name, offsetof(struct mail_namespace_settings, name), NULL }
#define DEFLIST_UNIQUE(field, name, defines) \
	{ SET_DEFLIST_UNIQUE, name, \
	  offsetof(struct mail_namespace_settings, field), defines }
static const struct setting_define mail_namespace_setting_defines[] = {
	DEF(SET_STR, name),
	DEF(SET_ENUM, type),
	DEF(SET_STR, separator),
	DEF(SET_STR_VARS, prefix),
	DEF(SET_STR_VARS, location),
	{ SET_ALIAS, "mail", 0, NULL },
	{ SET_ALIAS, "mail_location", 0, NULL },
	DEF(SET_STR_VARS, alias_for),

	DEF(SET_BOOL, inbox),
	DEF(SET_BOOL, hidden),
	DEF(SET_ENUM, list),
	DEF(SET_BOOL, subscriptions),
	DEF(SET_BOOL, ignore_on_failure),
	DEF(SET_BOOL, disabled),

	DEFLIST_UNIQUE(mailboxes, "mailbox", &mailbox_setting_parser_info),

	SETTING_DEFINE_LIST_END
};
const struct mail_namespace_settings mail_namespace_default_settings = {
	.name = "",
	.type = "private:shared:public",
	.separator = "",
	.prefix = "",
	.location = "",
	.alias_for = NULL,

	.inbox = FALSE,
	.hidden = FALSE,
	.list = "yes:no:children",
	.subscriptions = TRUE,
	.ignore_on_failure = FALSE,
	.disabled = FALSE,

	.mailboxes = ARRAY_INIT
};
const struct setting_parser_info mail_namespace_setting_parser_info = {
	.defines = mail_namespace_setting_defines,
	.defaults = &mail_namespace_default_settings,

	.type_offset = offsetof(struct mail_namespace_settings, name),
	.struct_size = sizeof(struct mail_namespace_settings),

	.parent_offset = offsetof(struct mail_namespace_settings, user_set),
	.parent = &mail_user_setting_parser_info,

	.check_func = namespace_settings_check
};
#undef DEF
#undef DEFLIST_UNIQUE
#define DEF(type, name) \
	{ type, #name, offsetof(struct mail_user_settings, name), NULL }
#define DEFLIST_UNIQUE(field, name, defines) \
	{ SET_DEFLIST_UNIQUE, name, \
	  offsetof(struct mail_user_settings, field), defines }
static const struct setting_define mail_user_setting_defines[] = {
	DEF(SET_STR, base_dir),
	DEF(SET_STR, auth_socket_path),
	DEF(SET_STR_VARS, mail_temp_dir),

	DEF(SET_STR, mail_uid),
	DEF(SET_STR, mail_gid),
	DEF(SET_STR_VARS, mail_home),
	DEF(SET_STR_VARS, mail_chroot),
	DEF(SET_STR, mail_access_groups),
	DEF(SET_STR, mail_privileged_group),
	DEF(SET_STR, valid_chroot_dirs),

	DEF(SET_UINT, first_valid_uid),
	DEF(SET_UINT, last_valid_uid),
	DEF(SET_UINT, first_valid_gid),
	DEF(SET_UINT, last_valid_gid),

	DEF(SET_STR, mail_plugins),
	DEF(SET_STR, mail_plugin_dir),

	DEF(SET_STR, mail_log_prefix),

	DEFLIST_UNIQUE(namespaces, "namespace", &mail_namespace_setting_parser_info),
	{ SET_STRLIST, "plugin", offsetof(struct mail_user_settings, plugin_envs), NULL },

	SETTING_DEFINE_LIST_END
};
static const struct mail_user_settings mail_user_default_settings = {
	.base_dir = PKG_RUNDIR,
	.auth_socket_path = "auth-userdb",
	.mail_temp_dir = "/tmp",

	.mail_uid = "",
	.mail_gid = "",
	.mail_home = "",
	.mail_chroot = "",
	.mail_access_groups = "",
	.mail_privileged_group = "",
	.valid_chroot_dirs = "",

	.first_valid_uid = 500,
	.last_valid_uid = 0,
	.first_valid_gid = 1,
	.last_valid_gid = 0,

	.mail_plugins = "",
	.mail_plugin_dir = MODULEDIR,

	.mail_log_prefix = "%s(%u): ",

	.namespaces = ARRAY_INIT,
	.plugin_envs = ARRAY_INIT
};
const struct setting_parser_info mail_user_setting_parser_info = {
	.module_name = "mail",
	.defines = mail_user_setting_defines,
	.defaults = &mail_user_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct mail_user_settings),

	.parent_offset = (size_t)-1,

	.check_func = mail_user_settings_check
};
/* ../../src/lib-storage/index/pop3c/pop3c-settings.c */
/* <settings checks> */
static bool pop3c_settings_check(void *_set, pool_t pool ATTR_UNUSED,
				 const char **error_r)
{
	struct pop3c_settings *set = _set;

	if (set->pop3c_port == 0 || set->pop3c_port > 65535) {
		*error_r = "invalid pop3c_port";
		return FALSE;
	}
	return TRUE;
}
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct pop3c_settings, name), NULL }
static const struct setting_define pop3c_setting_defines[] = {
	DEF(SET_STR, pop3c_host),
	DEF(SET_UINT, pop3c_port),

	DEF(SET_STR_VARS, pop3c_user),
	DEF(SET_STR_VARS, pop3c_master_user),
	DEF(SET_STR, pop3c_password),

	DEF(SET_ENUM, pop3c_ssl),
	DEF(SET_BOOL, pop3c_ssl_verify),

	DEF(SET_STR, pop3c_rawlog_dir),

	SETTING_DEFINE_LIST_END
};
static const struct pop3c_settings pop3c_default_settings = {
	.pop3c_host = "",
	.pop3c_port = 110,

	.pop3c_user = "%u",
	.pop3c_master_user = "",
	.pop3c_password = "",

	.pop3c_ssl = "no:pop3s:starttls",
	.pop3c_ssl_verify = TRUE,

	.pop3c_rawlog_dir = ""
};
static const struct setting_parser_info pop3c_setting_parser_info = {
	.module_name = "pop3c",
	.defines = pop3c_setting_defines,
	.defaults = &pop3c_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct pop3c_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info,

	.check_func = pop3c_settings_check
};
/* ../../src/lib-storage/index/mbox/mbox-settings.c */
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct mbox_settings, name), NULL }
static const struct setting_define mbox_setting_defines[] = {
	DEF(SET_STR, mbox_read_locks),
	DEF(SET_STR, mbox_write_locks),
	DEF(SET_TIME, mbox_lock_timeout),
	DEF(SET_TIME, mbox_dotlock_change_timeout),
	DEF(SET_SIZE, mbox_min_index_size),
	DEF(SET_BOOL, mbox_dirty_syncs),
	DEF(SET_BOOL, mbox_very_dirty_syncs),
	DEF(SET_BOOL, mbox_lazy_writes),
	DEF(SET_ENUM, mbox_md5),

	SETTING_DEFINE_LIST_END
};
static const struct mbox_settings mbox_default_settings = {
	.mbox_read_locks = "fcntl",
	.mbox_write_locks = "dotlock fcntl",
	.mbox_lock_timeout = 5*60,
	.mbox_dotlock_change_timeout = 2*60,
	.mbox_min_index_size = 0,
	.mbox_dirty_syncs = TRUE,
	.mbox_very_dirty_syncs = FALSE,
	.mbox_lazy_writes = TRUE,
	.mbox_md5 = "apop3d:all"
};
static const struct setting_parser_info mbox_setting_parser_info = {
	.module_name = "mbox",
	.defines = mbox_setting_defines,
	.defaults = &mbox_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct mbox_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info
};
/* ../../src/lib-storage/index/maildir/maildir-settings.c */
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct maildir_settings, name), NULL }
static const struct setting_define maildir_setting_defines[] = {
	DEF(SET_BOOL, maildir_copy_with_hardlinks),
	DEF(SET_BOOL, maildir_very_dirty_syncs),
	DEF(SET_BOOL, maildir_broken_filename_sizes),

	SETTING_DEFINE_LIST_END
};
static const struct maildir_settings maildir_default_settings = {
	.maildir_copy_with_hardlinks = TRUE,
	.maildir_very_dirty_syncs = FALSE,
	.maildir_broken_filename_sizes = FALSE
};
static const struct setting_parser_info maildir_setting_parser_info = {
	.module_name = "maildir",
	.defines = maildir_setting_defines,
	.defaults = &maildir_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct maildir_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info
};
/* ../../src/lib-storage/index/imapc/imapc-settings.c */
/* <settings checks> */
struct imapc_feature_list {
	const char *name;
	enum imapc_features num;
};

static const struct imapc_feature_list imapc_feature_list[] = {
	{ "rfc822.size", IMAPC_FEATURE_RFC822_SIZE },
	{ "guid-forced", IMAPC_FEATURE_GUID_FORCED },
	{ NULL, 0 }
};

static int
imapc_settings_parse_features(struct imapc_settings *set,
			      const char **error_r)
{
        enum imapc_features features = 0;
        const struct imapc_feature_list *list;
	const char *const *str;

        str = t_strsplit_spaces(set->imapc_features, " ,");
	for (; *str != NULL; str++) {
		list = imapc_feature_list;
		for (; list->name != NULL; list++) {
			if (strcasecmp(*str, list->name) == 0) {
				features |= list->num;
				break;
			}
		}
		if (list->name == NULL) {
			*error_r = t_strdup_printf("imapc_features: "
				"Unknown feature: %s", *str);
			return -1;
		}
	}
	set->parsed_features = features;
	return 0;
}

static bool imapc_settings_check(void *_set, pool_t pool ATTR_UNUSED,
				 const char **error_r)
{
	struct imapc_settings *set = _set;

	if (set->imapc_port == 0 || set->imapc_port > 65535) {
		*error_r = "invalid imapc_port";
		return FALSE;
	}
	if (set->imapc_max_idle_time == 0) {
		*error_r = "imapc_max_idle_time must not be 0";
		return FALSE;
	}
	if (imapc_settings_parse_features(set, error_r) < 0)
		return FALSE;
	return TRUE;
}
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct imapc_settings, name), NULL }
static const struct setting_define imapc_setting_defines[] = {
	DEF(SET_STR, imapc_host),
	DEF(SET_UINT, imapc_port),

	DEF(SET_STR_VARS, imapc_user),
	DEF(SET_STR_VARS, imapc_master_user),
	DEF(SET_STR, imapc_password),

	DEF(SET_ENUM, imapc_ssl),
	DEF(SET_BOOL, imapc_ssl_verify),

	DEF(SET_STR, imapc_features),
	DEF(SET_STR, imapc_rawlog_dir),
	DEF(SET_STR, imapc_list_prefix),
	DEF(SET_TIME, imapc_max_idle_time),

	SETTING_DEFINE_LIST_END
};
static const struct imapc_settings imapc_default_settings = {
	.imapc_host = "",
	.imapc_port = 143,

	.imapc_user = "",
	.imapc_master_user = "",
	.imapc_password = "",

	.imapc_ssl = "no:imaps:starttls",
	.imapc_ssl_verify = TRUE,

	.imapc_features = "",
	.imapc_rawlog_dir = "",
	.imapc_list_prefix = "",
	.imapc_max_idle_time = 60*29
};
static const struct setting_parser_info imapc_setting_parser_info = {
	.module_name = "imapc",
	.defines = imapc_setting_defines,
	.defaults = &imapc_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct imapc_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info,

	.check_func = imapc_settings_check
};
/* ../../src/lib-storage/index/dbox-multi/mdbox-settings.c */
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct mdbox_settings, name), NULL }
static const struct setting_define mdbox_setting_defines[] = {
	DEF(SET_BOOL, mdbox_preallocate_space),
	DEF(SET_SIZE, mdbox_rotate_size),
	DEF(SET_TIME, mdbox_rotate_interval),

	SETTING_DEFINE_LIST_END
};
static const struct mdbox_settings mdbox_default_settings = {
	.mdbox_preallocate_space = FALSE,
	.mdbox_rotate_size = 2*1024*1024,
	.mdbox_rotate_interval = 0
};
static const struct setting_parser_info mdbox_setting_parser_info = {
	.module_name = "mdbox",
	.defines = mdbox_setting_defines,
	.defaults = &mdbox_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct mdbox_settings),

	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info
};
/* ../../src/lib-settings/settings.c */
/* ../../src/lib-lda/lda-settings.c */
#undef DEF
#undef DEFLIST
#define DEF(type, name) \
	{ type, #name, offsetof(struct lda_settings, name), NULL }
#define DEFLIST(field, name, defines) \
	{ SET_DEFLIST, name, offsetof(struct lda_settings, field), defines }
static const struct setting_define lda_setting_defines[] = {
	DEF(SET_STR_VARS, postmaster_address),
	DEF(SET_STR, hostname),
	DEF(SET_STR, submission_host),
	DEF(SET_STR, sendmail_path),
	DEF(SET_STR, rejection_subject),
	DEF(SET_STR, rejection_reason),
	DEF(SET_STR, deliver_log_format),
	DEF(SET_STR, recipient_delimiter),
	DEF(SET_STR, lda_original_recipient_header),
	DEF(SET_BOOL, quota_full_tempfail),
	DEF(SET_BOOL, lda_mailbox_autocreate),
	DEF(SET_BOOL, lda_mailbox_listid_autosave),	/* Apple */
	DEF(SET_BOOL, lda_mailbox_autosubscribe),

	SETTING_DEFINE_LIST_END
};
static const struct lda_settings lda_default_settings = {
	.postmaster_address = "",
	.hostname = "",
	.submission_host = "",
	.sendmail_path = "/usr/sbin/sendmail",
	.rejection_subject = "Rejected: %s",
	.rejection_reason =
		"Your message to <%t> was automatically rejected:%n%r",
	.deliver_log_format = "msgid=%m: %$",
	.recipient_delimiter = "+",
	.lda_original_recipient_header = "",
	.quota_full_tempfail = FALSE,
	.lda_mailbox_autocreate = FALSE,
	.lda_mailbox_listid_autosave = FALSE,	/* Apple */
	.lda_mailbox_autosubscribe = FALSE
};
static const struct setting_parser_info *lda_setting_dependencies[] = {
	&mail_user_setting_parser_info,
	NULL
};
const struct setting_parser_info lda_setting_parser_info = {
	.module_name = "lda",
	.defines = lda_setting_defines,
	.defaults = &lda_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct lda_settings),

	.parent_offset = (size_t)-1,

#ifndef CONFIG_BINARY
	.check_func = lda_settings_check,
#endif
	.dependencies = lda_setting_dependencies
};
/* ../../src/lib-dict/dict-sql-settings.c */
#define DEF_STR(name) DEF_STRUCT_STR(name, dict_sql_map)
/* ../../src/stats/stats-settings.h */
extern const struct setting_parser_info stats_setting_parser_info;
struct stats_settings {
	uoff_t memory_limit;

	unsigned int command_min_time;
	unsigned int session_min_time;
	unsigned int user_min_time;
	unsigned int domain_min_time;
	unsigned int ip_min_time;
};
/* ../../src/ssl-params/ssl-params-settings.h */
struct ssl_params_settings {
	unsigned int ssl_parameters_regenerate;
};
/* ../../src/replication/replicator/replicator-settings.h */
extern const struct setting_parser_info replicator_setting_parser_info;
struct replicator_settings {
	const char *auth_socket_path;
	const char *doveadm_socket_path;

	unsigned int replication_full_sync_interval;
	unsigned int replication_max_conns;
};
/* ../../src/replication/aggregator/aggregator-settings.h */
extern const struct setting_parser_info aggregator_setting_parser_info;
struct aggregator_settings {
	const char *replicator_host;
	unsigned int replicator_port;
};
/* ../../src/pop3/pop3-settings.h */
extern const struct setting_parser_info pop3_setting_parser_info;
/* <settings checks> */
enum pop3_client_workarounds {
	WORKAROUND_OUTLOOK_NO_NULS		= 0x01,
	WORKAROUND_OE_NS_EOH			= 0x02
};
/* </settings checks> */
struct pop3_settings {
	bool verbose_proctitle;
	unsigned int mail_max_bad_commands;			/* APPLE */

	/* pop3: */
	bool pop3_no_flag_updates;
	bool pop3_enable_last;
	bool pop3_reuse_xuidl;
	bool pop3_save_uidl;
	bool pop3_lock_session;
	bool pop3_fast_size_lookups;
	const char *pop3_client_workarounds;
	const char *pop3_logout_format;
	const char *pop3_uidl_duplicates;
	const char *pop3_deleted_flag;

	enum pop3_client_workarounds parsed_workarounds;
};
/* ../../src/pop3-login/pop3-login-settings.h */
extern const struct setting_parser_info *pop3_login_setting_roots[];
/* ../../src/master/master-settings.h */
extern const struct setting_parser_info master_setting_parser_info;
struct master_settings {
	const char *base_dir;
	const char *state_dir;
	const char *libexec_dir;
	const char *instance_name;
	const char *import_environment;
	const char *protocols;
	const char *listen;
	const char *ssl;
	const char *default_internal_user;
	const char *default_login_user;
	unsigned int default_process_limit;
	unsigned int default_client_limit;
	unsigned int default_idle_kill;
	uoff_t default_vsz_limit;

	bool version_ignore;

	unsigned int first_valid_uid, last_valid_uid;
	unsigned int first_valid_gid, last_valid_gid;

	ARRAY_TYPE(service_settings) services;
	char **protocols_split;
};
/* ../../src/login-common/login-settings.h */
extern const struct setting_parser_info **login_set_roots;
extern const struct setting_parser_info login_setting_parser_info;
struct login_settings {
	const char *login_trusted_networks;
	const char *login_greeting;
	const char *login_log_format_elements, *login_log_format;
	const char *login_access_sockets;
	const char *director_username_hash;

	const char *ssl_client_cert;
	const char *ssl_client_key;
	bool ssl_require_crl;
	bool auth_ssl_require_client_cert;
	bool auth_ssl_username_from_cert;

	bool disable_plaintext_auth;
	bool auth_verbose;
	bool auth_debug;
	bool auth_debug_passwords;
	bool verbose_proctitle;

	unsigned int mail_max_userip_connections;

	/* generated: */
	char *const *log_format_elements_split;
};
/* ../../src/lmtp/lmtp-settings.h */
extern const struct setting_parser_info lmtp_setting_parser_info;
struct lmtp_settings {
	bool lmtp_proxy;
	bool lmtp_save_to_detail_mailbox;
	bool lmtp_rcpt_check_quota;
	const char *lmtp_address_translate;
	const char *login_greeting;
	const char *login_trusted_networks;
};
/* ../../src/imap/imap-settings.h */
extern const struct setting_parser_info imap_setting_parser_info;
/* <settings checks> */
enum imap_client_workarounds {
	WORKAROUND_DELAY_NEWMAIL		= 0x01,
	WORKAROUND_TB_EXTRA_MAILBOX_SEP		= 0x08,
	WORKAROUND_TB_LSUB_FLAGS		= 0x10
};
/* </settings checks> */
struct imap_settings {
	bool verbose_proctitle;
	unsigned int mail_max_bad_commands;			/* APPLE */

	/* imap: */
	uoff_t imap_max_line_length;
	unsigned int imap_idle_notify_interval;
	const char *imap_capability;
	const char *imap_client_workarounds;
	const char *imap_logout_format;
	const char *imap_id_send;
	const char *imap_id_log;
#ifdef APPLE_OS_X_SERVER
	const char *aps_topic;
#endif

	/* imap urlauth: */
	const char *imap_urlauth_host;
	unsigned int imap_urlauth_port;

	enum imap_client_workarounds parsed_workarounds;
};
/* ../../src/imap-urlauth/imap-urlauth-worker-settings.h */
extern const struct setting_parser_info imap_urlauth_worker_setting_parser_info;
struct imap_urlauth_worker_settings {
	bool verbose_proctitle;

	/* imap_urlauth: */
	const char *imap_urlauth_host;
	unsigned int imap_urlauth_port;
};
/* ../../src/imap-urlauth/imap-urlauth-settings.h */
extern const struct setting_parser_info imap_urlauth_setting_parser_info;
struct imap_urlauth_settings {
	const char *base_dir;

	bool mail_debug;

	bool verbose_proctitle;

	/* imap_urlauth: */
	const char *imap_urlauth_logout_format;

	const char *imap_urlauth_submit_user;
	const char *imap_urlauth_stream_user;
};
/* ../../src/imap-urlauth/imap-urlauth-login-settings.h */
extern const struct setting_parser_info *imap_urlauth_login_setting_roots[];
/* ../../src/imap-login/imap-login-settings.h */
extern const struct setting_parser_info *imap_login_setting_roots[];
struct imap_login_settings {
	const char *imap_capability;
	const char *imap_id_send;
	const char *imap_id_log;
#ifdef APPLE_OS_X_SERVER
	const char *aps_topic;
#endif
};
/* ../../src/doveadm/doveadm-settings.h */
extern const struct setting_parser_info doveadm_setting_parser_info;
struct doveadm_settings {
	const char *base_dir;
	const char *libexec_dir;
	const char *mail_plugins;
	const char *mail_plugin_dir;
	const char *doveadm_socket_path;
	unsigned int doveadm_worker_count;
	unsigned int doveadm_port;
	const char *doveadm_password;
	const char *doveadm_allowed_commands;
	const char *dsync_alt_char;
	const char *dsync_remote_cmd;
	const char *ssl_client_ca_dir;
	const char *ssl_client_ca_file;

	ARRAY(const char *) plugin_envs;
};
/* ../../src/director/director-settings.h */
extern const struct setting_parser_info director_setting_parser_info;
struct director_settings {
	const char *master_user_separator;

	const char *director_servers;
	const char *director_mail_servers;
	const char *director_username_hash;
	unsigned int director_user_expire;
	unsigned int director_doveadm_port;
};
/* ../../src/dict/dict-settings.h */
extern const struct setting_parser_info dict_setting_parser_info;
struct dict_settings {
	const char *base_dir;
	const char *dict_db_config;
	ARRAY(const char *) dicts;
};
/* ../../src/auth/auth-settings.h */
extern const struct setting_parser_info auth_setting_parser_info;
struct auth_passdb_settings {
	const char *driver;
	const char *args;
	const char *default_fields;
	const char *override_fields;
	const char *skip;
	const char *result_success;
	const char *result_failure;
	const char *result_internalfail;
	bool deny;
	bool pass;
	bool master;
};
struct auth_userdb_settings {
	const char *driver;
	const char *args;
	const char *default_fields;
	const char *override_fields;
};
struct auth_settings {
	const char *mechanisms;
	const char *realms;
	const char *default_realm;
	uoff_t cache_size;
	unsigned int cache_ttl;
	unsigned int cache_negative_ttl;
	const char *username_chars;
	const char *username_translation;
	const char *username_format;
	const char *master_user_separator;
	const char *anonymous_username;
	const char *krb5_keytab;
	const char *gssapi_hostname;
	const char *winbind_helper_path;
	const char *proxy_self;
	unsigned int failure_delay;

	bool verbose, debug, debug_passwords;
	const char *verbose_passwords;
	bool ssl_require_client_cert;
	bool ssl_username_from_cert;
	bool use_winbind;

	unsigned int worker_max_count;

	/* settings that don't have auth_ prefix: */
	ARRAY(struct auth_passdb_settings *) passdbs;
	ARRAY(struct auth_userdb_settings *) userdbs;

#ifdef APPLE_OS_X_SERVER
	const char *aps_topic;
#endif
	const char *base_dir;
	bool verbose_proctitle;
	unsigned int first_valid_uid;
	unsigned int last_valid_uid;

	/* generated: */
	char username_chars_map[256];
	char username_translation_map[256];
	const char *const *realms_arr;
	const struct ip_addr *proxy_self_ips;
};
/* ../../src/util/tcpwrap-settings.c */
#ifdef HAVE_LIBWRAP
struct service_settings tcpwrap_service_settings = {
	.name = "tcpwrap",
	.protocol = "",
	.type = "",
	.executable = "tcpwrap",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = ARRAY_INIT,
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#endif
/* ../../src/stats/stats-settings.c */
/* <settings checks> */
static struct file_listener_settings stats_unix_listeners_array[] = {
	{ "stats", 0600, "", "" }
};
static struct file_listener_settings *stats_unix_listeners[] = {
	&stats_unix_listeners_array[0]
};
static buffer_t stats_unix_listeners_buf = {
	stats_unix_listeners, sizeof(stats_unix_listeners), { 0, }
};
static struct file_listener_settings stats_fifo_listeners_array[] = {
	{ "stats-mail", 0600, "", "" }
};
static struct file_listener_settings *stats_fifo_listeners[] = {
	&stats_fifo_listeners_array[0]
};
static buffer_t stats_fifo_listeners_buf = {
	stats_fifo_listeners,
	sizeof(stats_fifo_listeners), { 0, }
};
/* </settings checks> */
struct service_settings stats_service_settings = {
	.name = "stats",
	.protocol = "",
	.type = "",
	.executable = "stats",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "empty",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = UINT_MAX,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &stats_unix_listeners_buf,
			      sizeof(stats_unix_listeners[0]) } },
	.fifo_listeners = { { &stats_fifo_listeners_buf,
			      sizeof(stats_fifo_listeners[0]) } },
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, "stats_"#name, offsetof(struct stats_settings, name), NULL }
static const struct setting_define stats_setting_defines[] = {
	DEF(SET_SIZE, memory_limit),
	DEF(SET_TIME, command_min_time),
	DEF(SET_TIME, session_min_time),
	DEF(SET_TIME, user_min_time),
	DEF(SET_TIME, domain_min_time),
	DEF(SET_TIME, ip_min_time),

	SETTING_DEFINE_LIST_END
};
const struct stats_settings stats_default_settings = {
	.memory_limit = 1024*1024*16,

	.command_min_time = 60,
	.session_min_time = 60*15,
	.user_min_time = 60*60,
	.domain_min_time = 60*60*12,
	.ip_min_time = 60*60*12
};
const struct setting_parser_info stats_setting_parser_info = {
	.module_name = "stats",
	.defines = stats_setting_defines,
	.defaults = &stats_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct stats_settings),

	.parent_offset = (size_t)-1
};
/* ../../src/ssl-params/ssl-params-settings.c */
/* <settings checks> */
static struct file_listener_settings ssl_params_unix_listeners_array[] = {
	{ "ssl-params", 0666, "", "" },
	{ "login/ssl-params", 0666, "", "" }
};
static struct file_listener_settings *ssl_params_unix_listeners[] = {
	&ssl_params_unix_listeners_array[0],
	&ssl_params_unix_listeners_array[1]
};
static buffer_t ssl_params_unix_listeners_buf = {
	ssl_params_unix_listeners, sizeof(ssl_params_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings ssl_params_service_settings = {
	.name = "ssl-params",
	.protocol = "",
#ifdef HAVE_SSL
	.type = "startup",
#else
	.type = "",
#endif
	.executable = "ssl-params",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &ssl_params_unix_listeners_buf,
			      sizeof(ssl_params_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct ssl_params_settings, name), NULL }
static const struct setting_define ssl_params_setting_defines[] = {
	DEF(SET_TIME, ssl_parameters_regenerate),

	SETTING_DEFINE_LIST_END
};
static const struct ssl_params_settings ssl_params_default_settings = {
	.ssl_parameters_regenerate = 3600*24*7
};
const struct setting_parser_info ssl_params_setting_parser_info = {
	.module_name = "ssl-params",
	.defines = ssl_params_setting_defines,
	.defaults = &ssl_params_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct ssl_params_settings),

	.parent_offset = (size_t)-1
};
/* ../../src/replication/replicator/replicator-settings.c */
/* <settings checks> */
static struct file_listener_settings replicator_unix_listeners_array[] = {
	{ "replicator", 0600, "$default_internal_user", "" },
	{ "replicator-doveadm", 0, "$default_internal_user", "" }
};
static struct file_listener_settings *replicator_unix_listeners[] = {
	&replicator_unix_listeners_array[0],
	&replicator_unix_listeners_array[1]
};
static buffer_t replicator_unix_listeners_buf = {
	replicator_unix_listeners, sizeof(replicator_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings replicator_service_settings = {
	.name = "replicator",
	.protocol = "",
	.type = "",
	.executable = "replicator",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = UINT_MAX,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &replicator_unix_listeners_buf,
			      sizeof(replicator_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct replicator_settings, name), NULL }
static const struct setting_define replicator_setting_defines[] = {
	DEF(SET_STR, auth_socket_path),
	DEF(SET_STR, doveadm_socket_path),

	DEF(SET_TIME, replication_full_sync_interval),
	DEF(SET_UINT, replication_max_conns),

	SETTING_DEFINE_LIST_END
};
const struct replicator_settings replicator_default_settings = {
	.auth_socket_path = "auth-userdb",
	.doveadm_socket_path = "doveadm-server",

	.replication_full_sync_interval = 60*60*24,
	.replication_max_conns = 10
};
const struct setting_parser_info replicator_setting_parser_info = {
	.module_name = "replicator",
	.defines = replicator_setting_defines,
	.defaults = &replicator_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct replicator_settings),

	.parent_offset = (size_t)-1
};
/* ../../src/replication/aggregator/aggregator-settings.c */
/* <settings checks> */
static struct file_listener_settings aggregator_unix_listeners_array[] = {
	{ "replication-notify", 0600, "", "" }
};
static struct file_listener_settings *aggregator_unix_listeners[] = {
	&aggregator_unix_listeners_array[0]
};
static buffer_t aggregator_unix_listeners_buf = {
	aggregator_unix_listeners, sizeof(aggregator_unix_listeners), { 0, }
};

static struct file_listener_settings aggregator_fifo_listeners_array[] = {
	{ "replication-notify-fifo", 0600, "", "" }
};
static struct file_listener_settings *aggregator_fifo_listeners[] = {
	&aggregator_fifo_listeners_array[0]
};
static buffer_t aggregator_fifo_listeners_buf = {
	aggregator_fifo_listeners, sizeof(aggregator_fifo_listeners), { 0, }
};
/* </settings checks> */
struct service_settings aggregator_service_settings = {
	.name = "aggregator",
	.protocol = "",
	.type = "",
	.executable = "aggregator",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = ".",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &aggregator_unix_listeners_buf,
			      sizeof(aggregator_unix_listeners[0]) } },
	.fifo_listeners = { { &aggregator_fifo_listeners_buf,
			      sizeof(aggregator_fifo_listeners[0]) } },
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct aggregator_settings, name), NULL }
static const struct setting_define aggregator_setting_defines[] = {
	DEF(SET_STR, replicator_host),
	DEF(SET_UINT, replicator_port),

	SETTING_DEFINE_LIST_END
};
const struct aggregator_settings aggregator_default_settings = {
	.replicator_host = "replicator",
	.replicator_port = 0
};
const struct setting_parser_info aggregator_setting_parser_info = {
	.module_name = "aggregator",
	.defines = aggregator_setting_defines,
	.defaults = &aggregator_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct aggregator_settings),

	.parent_offset = (size_t)-1
};
/* ../../src/pop3/pop3-settings.c */
/* <settings checks> */
static struct file_listener_settings pop3_unix_listeners_array[] = {
	{ "login/pop3", 0666, "", "" }
};
static struct file_listener_settings *pop3_unix_listeners[] = {
	&pop3_unix_listeners_array[0]
};
static buffer_t pop3_unix_listeners_buf = {
	pop3_unix_listeners, sizeof(pop3_unix_listeners), { 0, }
};
/* </settings checks> */
/* <settings checks> */
struct pop3_client_workaround_list {
	const char *name;
	enum pop3_client_workarounds num;
};

static const struct pop3_client_workaround_list pop3_client_workaround_list[] = {
	{ "outlook-no-nuls", WORKAROUND_OUTLOOK_NO_NULS },
	{ "oe-ns-eoh", WORKAROUND_OE_NS_EOH },
	{ NULL, 0 }
};

static int
pop3_settings_parse_workarounds(struct pop3_settings *set,
				const char **error_r)
{
        enum pop3_client_workarounds client_workarounds = 0;
	const struct pop3_client_workaround_list *list;
	const char *const *str;

        str = t_strsplit_spaces(set->pop3_client_workarounds, " ,");
	for (; *str != NULL; str++) {
		list = pop3_client_workaround_list;
		for (; list->name != NULL; list++) {
			if (strcasecmp(*str, list->name) == 0) {
				client_workarounds |= list->num;
				break;
			}
		}
		if (list->name == NULL) {
			*error_r = t_strdup_printf("pop3_client_workarounds: "
				"Unknown workaround: %s", *str);
			return -1;
		}
	}
	set->parsed_workarounds = client_workarounds;
	return 0;
}

static bool
pop3_settings_verify(void *_set, pool_t pool ATTR_UNUSED, const char **error_r)
{
	struct pop3_settings *set = _set;

	if (pop3_settings_parse_workarounds(set, error_r) < 0)
		return FALSE;
	return TRUE;
}
/* </settings checks> */
struct service_settings pop3_service_settings = {
	.name = "pop3",
	.protocol = "pop3",
	.type = "",
	.executable = "pop3",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1024,
	.client_limit = 1,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &pop3_unix_listeners_buf,
			      sizeof(pop3_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#undef DEFLIST
#define DEF(type, name) \
	{ type, #name, offsetof(struct pop3_settings, name), NULL }
#define DEFLIST(field, name, defines) \
	{ SET_DEFLIST, name, offsetof(struct pop3_settings, field), defines }
static const struct setting_define pop3_setting_defines[] = {
	DEF(SET_BOOL, verbose_proctitle),
	DEF(SET_UINT, mail_max_bad_commands),			/* APPLE */

	DEF(SET_BOOL, pop3_no_flag_updates),
	DEF(SET_BOOL, pop3_enable_last),
	DEF(SET_BOOL, pop3_reuse_xuidl),
	DEF(SET_BOOL, pop3_save_uidl),
	DEF(SET_BOOL, pop3_lock_session),
	DEF(SET_BOOL, pop3_fast_size_lookups),
	DEF(SET_STR, pop3_client_workarounds),
	DEF(SET_STR, pop3_logout_format),
	DEF(SET_ENUM, pop3_uidl_duplicates),
	DEF(SET_STR, pop3_deleted_flag),

	SETTING_DEFINE_LIST_END
};
static const struct pop3_settings pop3_default_settings = {
	.verbose_proctitle = FALSE,
	.mail_max_bad_commands = 20,			/* APPLE */

	.pop3_no_flag_updates = FALSE,
	.pop3_enable_last = FALSE,
	.pop3_reuse_xuidl = FALSE,
	.pop3_save_uidl = FALSE,
	.pop3_lock_session = FALSE,
	.pop3_fast_size_lookups = FALSE,
	.pop3_client_workarounds = "",
	.pop3_logout_format = "top=%t/%p, retr=%r/%b, del=%d/%m, size=%s",
	.pop3_uidl_duplicates = "allow:rename",
	.pop3_deleted_flag = ""
};
static const struct setting_parser_info *pop3_setting_dependencies[] = {
	&mail_user_setting_parser_info,
	NULL
};
const struct setting_parser_info pop3_setting_parser_info = {
	.module_name = "pop3",
	.defines = pop3_setting_defines,
	.defaults = &pop3_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct pop3_settings),

	.parent_offset = (size_t)-1,

	.check_func = pop3_settings_verify,
	.dependencies = pop3_setting_dependencies
};
/* ../../src/pop3-login/pop3-login-settings.c */
/* <settings checks> */
static struct inet_listener_settings pop3_login_inet_listeners_array[] = {
	{ "pop3", "", 110, FALSE },
	{ "pop3s", "", 995, TRUE }
};
static struct inet_listener_settings *pop3_login_inet_listeners[] = {
	&pop3_login_inet_listeners_array[0],
	&pop3_login_inet_listeners_array[1]
};
static buffer_t pop3_login_inet_listeners_buf = {
	pop3_login_inet_listeners, sizeof(pop3_login_inet_listeners), { 0, }
};

/* </settings checks> */
struct service_settings pop3_login_service_settings = {
	.name = "pop3-login",
	.protocol = "pop3",
	.type = "login",
	.executable = "pop3-login",
	.user = "$default_login_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "login",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = ARRAY_INIT,
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = { { &pop3_login_inet_listeners_buf,
			      sizeof(pop3_login_inet_listeners[0]) } }
};
static const struct setting_define pop3_login_setting_defines[] = {
	SETTING_DEFINE_LIST_END
};
static const struct setting_parser_info *pop3_login_setting_dependencies[] = {
	&login_setting_parser_info,
	NULL
};
const struct setting_parser_info pop3_login_setting_parser_info = {
	.module_name = "pop3-login",
	.defines = pop3_login_setting_defines,

	.type_offset = (size_t)-1,
	.parent_offset = (size_t)-1,

	.dependencies = pop3_login_setting_dependencies
};
const struct setting_parser_info *pop3_login_setting_roots[] = {
	&login_setting_parser_info,
	&pop3_login_setting_parser_info,
	NULL
};
/* ../../src/master/master-settings.c */
extern const struct setting_parser_info service_setting_parser_info;
extern const struct setting_parser_info service_setting_parser_info;
/* <settings checks> */
#ifdef HAVE_SYSTEMD
#  define ENV_SYSTEMD " LISTEN_PID LISTEN_FDS"
#else
#  define ENV_SYSTEMD ""
#endif
#ifdef DEBUG
#  define ENV_GDB " GDB DEBUG_SILENT"
#else
#  define ENV_GDB ""
#endif
/* </settings checks> */
/* <settings checks> */
static void
expand_user(const char **user, enum service_user_default *default_r,
	    const struct master_settings *set)
{
	/* $variable expansion is typically done by doveconf, but these
	   variables can come from built-in settings, so we need to expand
	   them here */
	if (strcmp(*user, "$default_internal_user") == 0) {
		*user = set->default_internal_user;
		*default_r = SERVICE_USER_DEFAULT_INTERNAL;
	} else if (strcmp(*user, "$default_login_user") == 0) {
		*user = set->default_login_user;
		*default_r = SERVICE_USER_DEFAULT_LOGIN;
	} else {
		*default_r = SERVICE_USER_DEFAULT_NONE;
	}
}

static void
fix_file_listener_paths(ARRAY_TYPE(file_listener_settings) *l,
			pool_t pool, const struct master_settings *master_set,
			ARRAY_TYPE(const_string) *all_listeners)
{
	struct file_listener_settings *const *sets;
	unsigned int base_dir_len = strlen(master_set->base_dir);
	enum service_user_default user_default;

	if (!array_is_created(l))
		return;

	array_foreach(l, sets) {
		struct file_listener_settings *set = *sets;

		expand_user(&set->user, &user_default, master_set);
		if (*set->path != '/') {
			set->path = p_strconcat(pool, master_set->base_dir, "/",
						set->path, NULL);
		} else if (strncmp(set->path, master_set->base_dir,
				   base_dir_len) == 0 &&
			   set->path[base_dir_len] == '/') {
			i_warning("You should remove base_dir prefix from "
				  "unix_listener: %s", set->path);
		}
		if (set->mode != 0)
			array_append(all_listeners, &set->path, 1);
	}
}

static void add_inet_listeners(ARRAY_TYPE(inet_listener_settings) *l,
			       ARRAY_TYPE(const_string) *all_listeners)
{
	struct inet_listener_settings *const *sets;
	const char *str;

	if (!array_is_created(l))
		return;

	array_foreach(l, sets) {
		struct inet_listener_settings *set = *sets;

		if (set->port != 0) {
			str = t_strdup_printf("%d:%s", set->port, set->address);
			array_append(all_listeners, &str, 1);
		}
	}
}

static bool master_settings_parse_type(struct service_settings *set,
				       const char **error_r)
{
	if (*set->type == '\0')
		set->parsed_type = SERVICE_TYPE_UNKNOWN;
	else if (strcmp(set->type, "log") == 0)
		set->parsed_type = SERVICE_TYPE_LOG;
	else if (strcmp(set->type, "config") == 0)
		set->parsed_type = SERVICE_TYPE_CONFIG;
	else if (strcmp(set->type, "anvil") == 0)
		set->parsed_type = SERVICE_TYPE_ANVIL;
	else if (strcmp(set->type, "login") == 0)
		set->parsed_type = SERVICE_TYPE_LOGIN;
	else if (strcmp(set->type, "startup") == 0)
		set->parsed_type = SERVICE_TYPE_STARTUP;
	else {
		*error_r = t_strconcat("Unknown service type: ",
				       set->type, NULL);
		return FALSE;
	}
	return TRUE;
}

static void service_set_login_dump_core(struct service_settings *set)
{
	const char *p;

	if (set->parsed_type != SERVICE_TYPE_LOGIN)
		return;

	p = strstr(set->executable, " -D");
	if (p != NULL && (p[3] == '\0' || p[3] == ' '))
		set->login_dump_core = TRUE;
}

static bool
services_have_protocol(struct master_settings *set, const char *name)
{
	struct service_settings *const *services;

	array_foreach(&set->services, services) {
		struct service_settings *service = *services;

		if (strcmp(service->protocol, name) == 0)
			return TRUE;
	}
	return FALSE;
}

#ifdef CONFIG_BINARY
static const struct service_settings *
master_default_settings_get_service(const char *name)
{
	extern struct master_settings master_default_settings;
	struct service_settings *const *setp;

	array_foreach(&master_default_settings.services, setp) {
		if (strcmp((*setp)->name, name) == 0)
			return *setp;
	}
	return NULL;
}
#endif

static unsigned int
service_get_client_limit(struct master_settings *set, const char *name)
{
	struct service_settings *const *servicep;

	array_foreach(&set->services, servicep) {
		if (strcmp((*servicep)->name, name) == 0) {
			if ((*servicep)->client_limit != 0)
				return (*servicep)->client_limit;
			else
				return set->default_client_limit;
		}
	}
	return set->default_client_limit;
}

static bool
master_settings_verify(void *_set, pool_t pool, const char **error_r)
{
	static int warned_auth = FALSE, warned_anvil = FALSE;
	struct master_settings *set = _set;
	struct service_settings *const *services;
	const char *const *strings;
	ARRAY_TYPE(const_string) all_listeners;
	struct passwd pw;
	unsigned int i, j, count, len, client_limit, process_limit;
	unsigned int max_auth_client_processes, max_anvil_client_processes;
#ifdef CONFIG_BINARY
	const struct service_settings *default_service;
#else
	rlim_t fd_limit;
	const char *max_client_limit_source = "default_client_limit";
	unsigned int max_client_limit = set->default_client_limit;
#endif

	len = strlen(set->base_dir);
	if (len > 0 && set->base_dir[len-1] == '/') {
		/* drop trailing '/' */
		set->base_dir = p_strndup(pool, set->base_dir, len - 1);
	}

	if (set->last_valid_uid != 0 &&
	    set->first_valid_uid > set->last_valid_uid) {
		*error_r = "first_valid_uid can't be larger than last_valid_uid";
		return FALSE;
	}
	if (set->last_valid_gid != 0 &&
	    set->first_valid_gid > set->last_valid_gid) {
		*error_r = "first_valid_gid can't be larger than last_valid_gid";
		return FALSE;
	}

	if (i_getpwnam(set->default_login_user, &pw) == 0) {
		*error_r = t_strdup_printf("default_login_user doesn't exist: %s",
					   set->default_login_user);
		return FALSE;
	}
	if (i_getpwnam(set->default_internal_user, &pw) == 0) {
		*error_r = t_strdup_printf("default_internal_user doesn't exist: %s",
					   set->default_internal_user);
		return FALSE;
	}

	/* check that we have at least one service. the actual service
	   structure validity is checked later while creating them. */
	if (!array_is_created(&set->services) ||
	    array_count(&set->services) == 0) {
		*error_r = "No services defined";
		return FALSE;
	}
	services = array_get(&set->services, &count);
	for (i = 0; i < count; i++) {
		struct service_settings *service = services[i];

		if (*service->name == '\0') {
			*error_r = t_strdup_printf(
				"Service #%d is missing name", i);
			return FALSE;
		}
		if (!master_settings_parse_type(service, error_r))
			return FALSE;
		for (j = 0; j < i; j++) {
			if (strcmp(service->name, services[j]->name) == 0) {
				*error_r = t_strdup_printf(
					"Duplicate service name: %s",
					service->name);
				return FALSE;
			}
		}
		expand_user(&service->user, &service->user_default, set);
		service_set_login_dump_core(service);
	}
	set->protocols_split = p_strsplit_spaces(pool, set->protocols, " ");
	if (set->protocols_split[0] != NULL &&
	    strcmp(set->protocols_split[0], "none") == 0 &&
	    set->protocols_split[1] == NULL)
		set->protocols_split[0] = NULL;

	for (i = 0; set->protocols_split[i] != NULL; i++) {
		if (!services_have_protocol(set, set->protocols_split[i])) {
			*error_r = t_strdup_printf("protocols: "
						   "Unknown protocol: %s",
						   set->protocols_split[i]);
			return FALSE;
		}
	}
	t_array_init(&all_listeners, 64);
	max_auth_client_processes = 0;
	max_anvil_client_processes = 2; /* blocking, nonblocking pipes */
	for (i = 0; i < count; i++) {
		struct service_settings *service = services[i];

		if (*service->protocol != '\0' &&
		    !str_array_find((const char **)set->protocols_split,
				    service->protocol)) {
			/* protocol not enabled, ignore its settings */
			continue;
		}

		if (*service->executable != '/' &&
		    *service->executable != '\0') {
			service->executable =
				p_strconcat(pool, set->libexec_dir, "/",
					    service->executable, NULL);
		}
		if (*service->chroot != '/' && *service->chroot != '\0') {
			service->chroot =
				p_strconcat(pool, set->base_dir, "/",
					    service->chroot, NULL);
		}
		if (service->drop_priv_before_exec &&
		    *service->chroot != '\0') {
			*error_r = t_strdup_printf("service(%s): "
				"drop_priv_before_exec=yes can't be "
				"used with chroot", service->name);
			return FALSE;
		}
		process_limit = service->process_limit;
		if (process_limit == 0)
			process_limit = set->default_process_limit;
		if (service->process_min_avail > process_limit) {
			*error_r = t_strdup_printf("service(%s): "
				"process_min_avail is higher than process_limit",
				service->name);
			return FALSE;
		}
		if (service->vsz_limit < 1024*1024 && service->vsz_limit != 0) {
			*error_r = t_strdup_printf("service(%s): "
				"vsz_limit is too low", service->name);
			return FALSE;
		}

#ifdef CONFIG_BINARY
		default_service =
			master_default_settings_get_service(service->name);
		if (default_service != NULL &&
		    default_service->process_limit_1 && process_limit > 1) {
			*error_r = t_strdup_printf("service(%s): "
				"process_limit must be 1", service->name);
			return FALSE;
		}
#else
		if (max_client_limit < service->client_limit) {
			max_client_limit = service->client_limit;
			max_client_limit_source = t_strdup_printf(
				"service %s { client_limit }", service->name);
		}
#endif

		if (*service->protocol != '\0') {
			/* each imap/pop3/lmtp process can use up a connection,
			   although if service_count=1 it's only temporary */
			if (service->service_count != 1 ||
			    strcmp(service->type, "login") == 0)
				max_auth_client_processes += process_limit;
		}
		if (strcmp(service->type, "login") == 0 ||
		    strcmp(service->name, "auth") == 0)
			max_anvil_client_processes += process_limit;

		fix_file_listener_paths(&service->unix_listeners,
					pool, set, &all_listeners);
		fix_file_listener_paths(&service->fifo_listeners,
					pool, set, &all_listeners);
		add_inet_listeners(&service->inet_listeners, &all_listeners);
	}

	client_limit = service_get_client_limit(set, "auth");
	if (client_limit < max_auth_client_processes && !warned_auth) {
		warned_auth = TRUE;
		i_warning("service auth { client_limit=%u } is lower than "
			  "required under max. load (%u)",
			  client_limit, max_auth_client_processes);
	}

	client_limit = service_get_client_limit(set, "anvil");
	if (client_limit < max_anvil_client_processes && !warned_anvil) {
		warned_anvil = TRUE;
		i_warning("service anvil { client_limit=%u } is lower than "
			  "required under max. load (%u)",
			  client_limit, max_anvil_client_processes);
	}
#ifndef CONFIG_BINARY
	if (restrict_get_fd_limit(&fd_limit) == 0 &&
	    fd_limit < (rlim_t)max_client_limit) {
		i_warning("fd limit (ulimit -n) is lower than required "
			  "under max. load (%u < %u), because of %s",
			  (unsigned int)fd_limit, max_client_limit,
			  max_client_limit_source);
	}
#endif

	/* check for duplicate listeners */
	array_sort(&all_listeners, i_strcmp_p);
	strings = array_get(&all_listeners, &count);
	for (i = 1; i < count; i++) {
		if (strcmp(strings[i-1], strings[i]) == 0) {
			*error_r = t_strdup_printf("duplicate listener: %s",
						   strings[i]);
			return FALSE;
		}
	}
	return TRUE;
}
/* </settings checks> */
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct file_listener_settings, name), NULL }
static const struct setting_define file_listener_setting_defines[] = {
	DEF(SET_STR, path),
	DEF(SET_UINT_OCT, mode),
	DEF(SET_STR, user),
	DEF(SET_STR, group),

	SETTING_DEFINE_LIST_END
};
static const struct file_listener_settings file_listener_default_settings = {
	.path = "",
	.mode = 0600,
	.user = "",
	.group = "",
};
static const struct setting_parser_info file_listener_setting_parser_info = {
	.defines = file_listener_setting_defines,
	.defaults = &file_listener_default_settings,

	.type_offset = offsetof(struct file_listener_settings, path),
	.struct_size = sizeof(struct file_listener_settings),

	.parent_offset = (size_t)-1,
	.parent = &service_setting_parser_info
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct inet_listener_settings, name), NULL }
static const struct setting_define inet_listener_setting_defines[] = {
	DEF(SET_STR, name),
	DEF(SET_STR, address),
	DEF(SET_UINT, port),
	DEF(SET_BOOL, ssl),

	SETTING_DEFINE_LIST_END
};
static const struct inet_listener_settings inet_listener_default_settings = {
	.name = "",
	.address = "",
	.port = 0,
	.ssl = FALSE
};
static const struct setting_parser_info inet_listener_setting_parser_info = {
	.defines = inet_listener_setting_defines,
	.defaults = &inet_listener_default_settings,

	.type_offset = offsetof(struct inet_listener_settings, name),
	.struct_size = sizeof(struct inet_listener_settings),

	.parent_offset = (size_t)-1,
	.parent = &service_setting_parser_info
};
#undef DEF
#undef DEFLIST
#undef DEFLIST_UNIQUE
#define DEF(type, name) \
	{ type, #name, offsetof(struct service_settings, name), NULL }
#define DEFLIST(field, name, defines) \
	{ SET_DEFLIST, name, offsetof(struct service_settings, field), defines }
#define DEFLIST_UNIQUE(field, name, defines) \
	{ SET_DEFLIST_UNIQUE, name, offsetof(struct service_settings, field), defines }
static const struct setting_define service_setting_defines[] = {
	DEF(SET_STR, name),
	DEF(SET_STR, protocol),
	DEF(SET_STR, type),
	DEF(SET_STR, executable),
	DEF(SET_STR, user),
	DEF(SET_STR, group),
	DEF(SET_STR, privileged_group),
	DEF(SET_STR, extra_groups),
	DEF(SET_STR, chroot),

	DEF(SET_BOOL, drop_priv_before_exec),

	DEF(SET_UINT, process_min_avail),
	DEF(SET_UINT, process_limit),
	DEF(SET_UINT, client_limit),
	DEF(SET_UINT, service_count),
	DEF(SET_TIME, idle_kill),
	DEF(SET_SIZE, vsz_limit),

	DEFLIST_UNIQUE(unix_listeners, "unix_listener",
		       &file_listener_setting_parser_info),
	DEFLIST_UNIQUE(fifo_listeners, "fifo_listener",
		       &file_listener_setting_parser_info),
	DEFLIST_UNIQUE(inet_listeners, "inet_listener",
		       &inet_listener_setting_parser_info),

	SETTING_DEFINE_LIST_END
};
static const struct service_settings service_default_settings = {
	.name = "",
	.protocol = "",
	.type = "",
	.executable = "",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = ARRAY_INIT,
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
const struct setting_parser_info service_setting_parser_info = {
	.defines = service_setting_defines,
	.defaults = &service_default_settings,

	.type_offset = offsetof(struct service_settings, name),
	.struct_size = sizeof(struct service_settings),

	.parent_offset = offsetof(struct service_settings, master_set),
	.parent = &master_setting_parser_info
};
#undef DEF
#undef DEFLIST_UNIQUE
#define DEF(type, name) \
	{ type, #name, offsetof(struct master_settings, name), NULL }
#define DEFLIST_UNIQUE(field, name, defines) \
	{ SET_DEFLIST_UNIQUE, name, offsetof(struct master_settings, field), defines }
static const struct setting_define master_setting_defines[] = {
	DEF(SET_STR, base_dir),
	DEF(SET_STR, state_dir),
	DEF(SET_STR, libexec_dir),
	DEF(SET_STR, instance_name),
	DEF(SET_STR, import_environment),
	DEF(SET_STR, protocols),
	DEF(SET_STR, listen),
	DEF(SET_ENUM, ssl),
	DEF(SET_STR, default_internal_user),
	DEF(SET_STR, default_login_user),
	DEF(SET_UINT, default_process_limit),
	DEF(SET_UINT, default_client_limit),
	DEF(SET_TIME, default_idle_kill),
	DEF(SET_SIZE, default_vsz_limit),

	DEF(SET_BOOL, version_ignore),

	DEF(SET_UINT, first_valid_uid),
	DEF(SET_UINT, last_valid_uid),
	DEF(SET_UINT, first_valid_gid),
	DEF(SET_UINT, last_valid_gid),

	DEFLIST_UNIQUE(services, "service", &service_setting_parser_info),

	SETTING_DEFINE_LIST_END
};
struct master_settings master_default_settings = {
	.base_dir = PKG_RUNDIR,
	.state_dir = PKG_STATEDIR,
	.libexec_dir = PKG_LIBEXECDIR,
	.instance_name = PACKAGE,
	.import_environment = "TZ" ENV_SYSTEMD ENV_GDB,
	.protocols = "imap pop3 lmtp",
	.listen = "*, ::",
	.ssl = "yes:no:required",
	.default_internal_user = "dovecot",
	.default_login_user = "dovenull",
	.default_process_limit = 100,
	.default_client_limit = 1000,
	.default_idle_kill = 60,
	.default_vsz_limit = 256*1024*1024,

	.version_ignore = FALSE,

	.first_valid_uid = 500,
	.last_valid_uid = 0,
	.first_valid_gid = 1,
	.last_valid_gid = 0,

#ifndef CONFIG_BINARY
	.services = ARRAY_INIT
#else
	.services = { { &config_all_services_buf,
			     sizeof(struct service_settings *) } },
#endif
};
const struct setting_parser_info master_setting_parser_info = {
	.module_name = "master",
	.defines = master_setting_defines,
	.defaults = &master_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct master_settings),

	.parent_offset = (size_t)-1,

	.check_func = master_settings_verify
};
/* ../../src/login-common/login-settings.c */
/* <settings checks> */
static bool login_settings_check(void *_set, pool_t pool,
				 const char **error_r ATTR_UNUSED)
{
	struct login_settings *set = _set;

	set->log_format_elements_split =
		p_strsplit(pool, set->login_log_format_elements, " ");

	if (set->auth_debug_passwords)
		set->auth_debug = TRUE;
	if (set->auth_debug)
		set->auth_verbose = TRUE;
	return TRUE;
}
/* </settings checks> */
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct login_settings, name), NULL }
static const struct setting_define login_setting_defines[] = {
	DEF(SET_STR, login_trusted_networks),
	DEF(SET_STR_VARS, login_greeting),
	DEF(SET_STR, login_log_format_elements),
	DEF(SET_STR, login_log_format),
	DEF(SET_STR, login_access_sockets),
	DEF(SET_STR, director_username_hash),

	DEF(SET_STR, ssl_client_cert),
	DEF(SET_STR, ssl_client_key),
	DEF(SET_BOOL, ssl_require_crl),
	DEF(SET_BOOL, auth_ssl_require_client_cert),
	DEF(SET_BOOL, auth_ssl_username_from_cert),

	DEF(SET_BOOL, disable_plaintext_auth),
	DEF(SET_BOOL, auth_verbose),
	DEF(SET_BOOL, auth_debug),
	DEF(SET_BOOL, verbose_proctitle),

	DEF(SET_UINT, mail_max_userip_connections),

	SETTING_DEFINE_LIST_END
};
static const struct login_settings login_default_settings = {
	.login_trusted_networks = "",
	.login_greeting = PACKAGE_NAME" ready.",
	.login_log_format_elements = "user=<%u> method=%m rip=%r lip=%l mpid=%e %c session=<%{session}>",
	.login_log_format = "%$: %s",
	.login_access_sockets = "",
	.director_username_hash = "%u",

	.ssl_client_cert = "",
	.ssl_client_key = "",
	.ssl_require_crl = TRUE,
	.auth_ssl_require_client_cert = FALSE,
	.auth_ssl_username_from_cert = FALSE,

	.disable_plaintext_auth = TRUE,
	.auth_verbose = FALSE,
	.auth_debug = FALSE,
	.verbose_proctitle = FALSE,

	.mail_max_userip_connections = 10
};
const struct setting_parser_info login_setting_parser_info = {
	.module_name = "login",
	.defines = login_setting_defines,
	.defaults = &login_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct login_settings),

	.parent_offset = (size_t)-1,

	.check_func = login_settings_check
};
/* ../../src/log/log-settings.c */
/* <settings checks> */
static struct file_listener_settings log_unix_listeners_array[] = {
	{ "log-errors", 0600, "", "" }
};
static struct file_listener_settings *log_unix_listeners[] = {
	&log_unix_listeners_array[0]
};
static buffer_t log_unix_listeners_buf = {
	log_unix_listeners,
	sizeof(log_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings log_service_settings = {
	.name = "log",
	.protocol = "",
	.type = "log",
	.executable = "log",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = UINT_MAX,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &log_unix_listeners_buf,
			      sizeof(log_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT,

	.process_limit_1 = TRUE
};
/* ../../src/lmtp/lmtp-settings.c */
/* <settings checks> */
static struct file_listener_settings lmtp_unix_listeners_array[] = {
	{ "lmtp", 0666, "", "" }
};
static struct file_listener_settings *lmtp_unix_listeners[] = {
	&lmtp_unix_listeners_array[0]
};
static buffer_t lmtp_unix_listeners_buf = {
	lmtp_unix_listeners, sizeof(lmtp_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings lmtp_service_settings = {
	.name = "lmtp",
	.protocol = "lmtp",
	.type = "",
	.executable = "lmtp",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &lmtp_unix_listeners_buf,
			      sizeof(lmtp_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct lmtp_settings, name), NULL }
static const struct setting_define lmtp_setting_defines[] = {
	DEF(SET_BOOL, lmtp_proxy),
	DEF(SET_BOOL, lmtp_save_to_detail_mailbox),
	DEF(SET_BOOL, lmtp_rcpt_check_quota),
	DEF(SET_STR, lmtp_address_translate),
	DEF(SET_STR_VARS, login_greeting),
	DEF(SET_STR, login_trusted_networks),

	SETTING_DEFINE_LIST_END
};
static const struct lmtp_settings lmtp_default_settings = {
	.lmtp_proxy = FALSE,
	.lmtp_save_to_detail_mailbox = FALSE,
	.lmtp_rcpt_check_quota = FALSE,
	.lmtp_address_translate = "",
	.login_greeting = PACKAGE_NAME" ready.",
	.login_trusted_networks = ""
};
static const struct setting_parser_info *lmtp_setting_dependencies[] = {
	&lda_setting_parser_info,
	NULL
};
const struct setting_parser_info lmtp_setting_parser_info = {
	.module_name = "lmtp",
	.defines = lmtp_setting_defines,
	.defaults = &lmtp_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct lmtp_settings),

	.parent_offset = (size_t)-1,

	.dependencies = lmtp_setting_dependencies
};
/* ../../src/ipc/ipc-settings.c */
/* <settings checks> */
static struct file_listener_settings ipc_unix_listeners_array[] = {
	{ "ipc", 0600, "", "" },
	{ "login/ipc-proxy", 0600, "$default_login_user", "" }
};
static struct file_listener_settings *ipc_unix_listeners[] = {
	&ipc_unix_listeners_array[0],
	&ipc_unix_listeners_array[1]
};
static buffer_t ipc_unix_listeners_buf = {
	ipc_unix_listeners, sizeof(ipc_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings ipc_service_settings = {
	.name = "ipc",
	.protocol = "",
	.type = "",
	.executable = "ipc",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "empty",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &ipc_unix_listeners_buf,
			      sizeof(ipc_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
/* ../../src/indexer/indexer-worker-settings.c */
/* <settings checks> */
static struct file_listener_settings indexer_worker_unix_listeners_array[] = {
	{ "indexer-worker", 0600, "$default_internal_user", "" }
};
static struct file_listener_settings *indexer_worker_unix_listeners[] = {
	&indexer_worker_unix_listeners_array[0]
};
static buffer_t indexer_worker_unix_listeners_buf = {
	indexer_worker_unix_listeners, sizeof(indexer_worker_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings indexer_worker_service_settings = {
	.name = "indexer-worker",
	.protocol = "",
	.type = "",
	.executable = "indexer-worker",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 10,
	.client_limit = 1,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &indexer_worker_unix_listeners_buf,
			      sizeof(indexer_worker_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
/* ../../src/indexer/indexer-settings.c */
extern const struct setting_parser_info service_setting_parser_info;
/* <settings checks> */
static struct file_listener_settings indexer_unix_listeners_array[] = {
	{ "indexer", 0666, "", "" }
};
static struct file_listener_settings *indexer_unix_listeners[] = {
	&indexer_unix_listeners_array[0]
};
static buffer_t indexer_unix_listeners_buf = {
	indexer_unix_listeners, sizeof(indexer_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings indexer_service_settings = {
	.name = "indexer",
	.protocol = "",
	.type = "",
	.executable = "indexer",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &indexer_unix_listeners_buf,
			      sizeof(indexer_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
/* ../../src/imap/imap-settings.c */
/* <settings checks> */
static struct file_listener_settings imap_unix_listeners_array[] = {
	{ "login/imap", 0666, "", "" }
};
static struct file_listener_settings *imap_unix_listeners[] = {
	&imap_unix_listeners_array[0]
};
static buffer_t imap_unix_listeners_buf = {
	imap_unix_listeners, sizeof(imap_unix_listeners), { 0, }
};
/* </settings checks> */
/* <settings checks> */
struct imap_client_workaround_list {
	const char *name;
	enum imap_client_workarounds num;
};

static const struct imap_client_workaround_list imap_client_workaround_list[] = {
	{ "delay-newmail", WORKAROUND_DELAY_NEWMAIL },
	{ "tb-extra-mailbox-sep", WORKAROUND_TB_EXTRA_MAILBOX_SEP },
	{ "tb-lsub-flags", WORKAROUND_TB_LSUB_FLAGS },
	{ NULL, 0 }
};

static int
imap_settings_parse_workarounds(struct imap_settings *set,
				const char **error_r)
{
        enum imap_client_workarounds client_workarounds = 0;
        const struct imap_client_workaround_list *list;
	const char *const *str;

        str = t_strsplit_spaces(set->imap_client_workarounds, " ,");
	for (; *str != NULL; str++) {
		list = imap_client_workaround_list;
		for (; list->name != NULL; list++) {
			if (strcasecmp(*str, list->name) == 0) {
				client_workarounds |= list->num;
				break;
			}
		}
		if (list->name == NULL) {
			*error_r = t_strdup_printf("imap_client_workarounds: "
				"Unknown workaround: %s", *str);
			return -1;
		}
	}
	set->parsed_workarounds = client_workarounds;
	return 0;
}


static bool
imap_settings_verify(void *_set, pool_t pool ATTR_UNUSED, const char **error_r)
{
	struct imap_settings *set = _set;

	if (imap_settings_parse_workarounds(set, error_r) < 0)
		return FALSE;
	return TRUE;
}
/* </settings checks> */
struct service_settings imap_service_settings = {
	.name = "imap",
	.protocol = "imap",
	.type = "",
	.executable = "imap",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1024,
	.client_limit = 1,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &imap_unix_listeners_buf,
			      sizeof(imap_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#undef DEFLIST
#define DEF(type, name) \
	{ type, #name, offsetof(struct imap_settings, name), NULL }
#define DEFLIST(field, name, defines) \
	{ SET_DEFLIST, name, offsetof(struct imap_settings, field), defines }
static const struct setting_define imap_setting_defines[] = {
	DEF(SET_BOOL, verbose_proctitle),
	DEF(SET_UINT, mail_max_bad_commands),			/* APPLE */

	DEF(SET_SIZE, imap_max_line_length),
	DEF(SET_TIME, imap_idle_notify_interval),
	DEF(SET_STR, imap_capability),
	DEF(SET_STR, imap_client_workarounds),
	DEF(SET_STR, imap_logout_format),
	DEF(SET_STR, imap_id_send),
	DEF(SET_STR, imap_id_log),
#ifdef APPLE_OS_X_SERVER
	DEF(SET_STR, aps_topic),
#endif

	DEF(SET_STR, imap_urlauth_host),
	DEF(SET_UINT, imap_urlauth_port),

	SETTING_DEFINE_LIST_END
};
static const struct imap_settings imap_default_settings = {
	.verbose_proctitle = FALSE,
	.mail_max_bad_commands = 20,			/* APPLE */

	/* RFC-2683 recommends at least 8000 bytes. Some clients however don't
	   break large message sets to multiple commands, so we're pretty
	   liberal by default. */
	.imap_max_line_length = 64*1024,
	.imap_idle_notify_interval = 2*60,
	.imap_capability = "",
	.imap_client_workarounds = "",
	.imap_logout_format = "in=%i out=%o",
	.imap_id_send = "name *",
	.imap_id_log = "",
#ifdef APPLE_OS_X_SERVER
	.aps_topic = "",
#endif

	.imap_urlauth_host = "",
	.imap_urlauth_port = 143
};
static const struct setting_parser_info *imap_setting_dependencies[] = {
	&mail_user_setting_parser_info,
	NULL
};
const struct setting_parser_info imap_setting_parser_info = {
	.module_name = "imap",
	.defines = imap_setting_defines,
	.defaults = &imap_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct imap_settings),

	.parent_offset = (size_t)-1,

	.check_func = imap_settings_verify,
	.dependencies = imap_setting_dependencies
};
/* ../../src/imap-urlauth/imap-urlauth-worker-settings.c */
/* <settings checks> */
static struct file_listener_settings imap_urlauth_worker_unix_listeners_array[] = {
	{ "imap-urlauth-worker", 0600, "$default_internal_user", "" }
};
static struct file_listener_settings *imap_urlauth_worker_unix_listeners[] = {
	&imap_urlauth_worker_unix_listeners_array[0]
};
static buffer_t imap_urlauth_worker_unix_listeners_buf = {
	imap_urlauth_worker_unix_listeners, sizeof(imap_urlauth_worker_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings imap_urlauth_worker_service_settings = {
	.name = "imap-urlauth-worker",
	.protocol = "imap",
	.type = "",
	.executable = "imap-urlauth-worker",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1024,
	.client_limit = 1,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &imap_urlauth_worker_unix_listeners_buf,
			      sizeof(imap_urlauth_worker_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct imap_urlauth_worker_settings, name), NULL }
static const struct setting_define imap_urlauth_worker_setting_defines[] = {
	DEF(SET_BOOL, verbose_proctitle),

	DEF(SET_STR, imap_urlauth_host),
	DEF(SET_UINT, imap_urlauth_port),

	SETTING_DEFINE_LIST_END
};
const struct imap_urlauth_worker_settings imap_urlauth_worker_default_settings = {
	.verbose_proctitle = FALSE,

	.imap_urlauth_host = "",
	.imap_urlauth_port = 143
};
static const struct setting_parser_info *imap_urlauth_worker_setting_dependencies[] = {
	&mail_user_setting_parser_info,
	NULL
};
const struct setting_parser_info imap_urlauth_worker_setting_parser_info = {
	.module_name = "imap-urlauth-worker",
	.defines = imap_urlauth_worker_setting_defines,
	.defaults = &imap_urlauth_worker_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct imap_urlauth_worker_settings),

	.parent_offset = (size_t)-1,

	.dependencies = imap_urlauth_worker_setting_dependencies
};
/* ../../src/imap-urlauth/imap-urlauth-settings.c */
/* <settings checks> */
static struct file_listener_settings imap_urlauth_unix_listeners_array[] = {
	{ "token-login/imap-urlauth", 0666, "", "" }
};
static struct file_listener_settings *imap_urlauth_unix_listeners[] = {
	&imap_urlauth_unix_listeners_array[0]
};
static buffer_t imap_urlauth_unix_listeners_buf = {
	imap_urlauth_unix_listeners, sizeof(imap_urlauth_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings imap_urlauth_service_settings = {
	.name = "imap-urlauth",
	.protocol = "imap",
	.type = "",
	.executable = "imap-urlauth",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1024,
	.client_limit = 1,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &imap_urlauth_unix_listeners_buf,
			      sizeof(imap_urlauth_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct imap_urlauth_settings, name), NULL }
static const struct setting_define imap_urlauth_setting_defines[] = {
	DEF(SET_STR, base_dir),

	DEF(SET_BOOL, mail_debug),

	DEF(SET_BOOL, verbose_proctitle),

	DEF(SET_STR, imap_urlauth_logout_format),
	DEF(SET_STR, imap_urlauth_submit_user),
	DEF(SET_STR, imap_urlauth_stream_user),

	SETTING_DEFINE_LIST_END
};
const struct imap_urlauth_settings imap_urlauth_default_settings = {
	.base_dir = PKG_RUNDIR,
  .mail_debug = FALSE,

	.verbose_proctitle = FALSE,

	.imap_urlauth_logout_format = "in=%i out=%o",
	.imap_urlauth_submit_user = NULL,
	.imap_urlauth_stream_user = NULL
};
static const struct setting_parser_info *imap_urlauth_setting_dependencies[] = {
	NULL
};
const struct setting_parser_info imap_urlauth_setting_parser_info = {
	.module_name = "imap-urlauth",
	.defines = imap_urlauth_setting_defines,
	.defaults = &imap_urlauth_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct imap_urlauth_settings),

	.parent_offset = (size_t)-1,

	.dependencies = imap_urlauth_setting_dependencies
};
/* ../../src/imap-urlauth/imap-urlauth-login-settings.c */
/* <settings checks> */
static struct file_listener_settings
imap_urlauth_login_unix_listeners_array[] = {
	{ "imap-urlauth", 0666, "", "" }
};
static struct file_listener_settings *imap_urlauth_login_unix_listeners[] = {
	&imap_urlauth_login_unix_listeners_array[0]
};
static buffer_t imap_urlauth_login_unix_listeners_buf = {
	imap_urlauth_login_unix_listeners,
		sizeof(imap_urlauth_login_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings imap_urlauth_login_service_settings = {
	.name = "imap-urlauth-login",
	.protocol = "imap",
	.type = "login",
	.executable = "imap-urlauth-login",
	.user = "$default_login_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "token-login",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &imap_urlauth_login_unix_listeners_buf,
			      sizeof(imap_urlauth_login_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
static const struct setting_define imap_urlauth_login_setting_defines[] = {
	SETTING_DEFINE_LIST_END
};
static const struct setting_parser_info *imap_urlauth_login_setting_dependencies[] = {
	&login_setting_parser_info,
	NULL
};
const struct setting_parser_info imap_urlauth_login_setting_parser_info = {
	.module_name = "imap-urlauth-login",
	.defines = imap_urlauth_login_setting_defines,

	.type_offset = (size_t)-1,
	.parent_offset = (size_t)-1,

	.dependencies = imap_urlauth_login_setting_dependencies
};
const struct setting_parser_info *imap_urlauth_login_setting_roots[] = {
	&login_setting_parser_info,
	&imap_urlauth_login_setting_parser_info,
	NULL
};
/* ../../src/imap-login/imap-login-settings.c */
/* <settings checks> */
static struct inet_listener_settings imap_login_inet_listeners_array[] = {
	{ "imap", "", 143, FALSE },
	{ "imaps", "", 993, TRUE }
};
static struct inet_listener_settings *imap_login_inet_listeners[] = {
	&imap_login_inet_listeners_array[0],
	&imap_login_inet_listeners_array[1]
};
static buffer_t imap_login_inet_listeners_buf = {
	imap_login_inet_listeners, sizeof(imap_login_inet_listeners), { 0, }
};
/* </settings checks> */
struct service_settings imap_login_service_settings = {
	.name = "imap-login",
	.protocol = "imap",
	.type = "login",
	.executable = "imap-login",
	.user = "$default_login_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "login",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = ARRAY_INIT,
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = { { &imap_login_inet_listeners_buf,
			      sizeof(imap_login_inet_listeners[0]) } }
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct imap_login_settings, name), NULL }
static const struct setting_define imap_login_setting_defines[] = {
	DEF(SET_STR, imap_capability),
	DEF(SET_STR, imap_id_send),
	DEF(SET_STR, imap_id_log),
#ifdef APPLE_OS_X_SERVER
	DEF(SET_STR, aps_topic),
#endif

	SETTING_DEFINE_LIST_END
};
static const struct imap_login_settings imap_login_default_settings = {
	.imap_capability = "",
	.imap_id_send = "name *",
	.imap_id_log = "",
#ifdef APPLE_OS_X_SERVER
	.aps_topic = ""
#endif
};
static const struct setting_parser_info *imap_login_setting_dependencies[] = {
	&login_setting_parser_info,
	NULL
};
static const struct setting_parser_info imap_login_setting_parser_info = {
	.module_name = "imap-login",
	.defines = imap_login_setting_defines,
	.defaults = &imap_login_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct imap_login_settings),

	.parent_offset = (size_t)-1,
	.dependencies = imap_login_setting_dependencies
};
const struct setting_parser_info *imap_login_setting_roots[] = {
	&login_setting_parser_info,
	&imap_login_setting_parser_info,
	NULL
};
/* ../../src/doveadm/doveadm-settings.c */
/* <settings checks> */
static struct file_listener_settings doveadm_unix_listeners_array[] = {
	{ "doveadm-server", 0600, "", "" }
};
static struct file_listener_settings *doveadm_unix_listeners[] = {
	&doveadm_unix_listeners_array[0]
};
static buffer_t doveadm_unix_listeners_buf = {
	doveadm_unix_listeners, sizeof(doveadm_unix_listeners), { 0, }
};
/* </settings checks> */
/* <settings checks> */
static bool doveadm_settings_check(void *_set, pool_t pool ATTR_UNUSED,
				   const char **error_r)
{
	struct doveadm_settings *set = _set;

#ifndef CONFIG_BINARY
	fix_base_path(set, pool, &set->doveadm_socket_path);
#endif
	if (*set->dsync_alt_char == '\0') {
		*error_r = "dsync_alt_char must not be empty";
		return FALSE;
	}
	return TRUE;
}
/* </settings checks> */
struct service_settings doveadm_service_settings = {
	.name = "doveadm",
	.protocol = "",
	.type = "",
	.executable = "doveadm-server",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &doveadm_unix_listeners_buf,
			      sizeof(doveadm_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct doveadm_settings, name), NULL }
static const struct setting_define doveadm_setting_defines[] = {
	DEF(SET_STR, base_dir),
	DEF(SET_STR, libexec_dir),
	DEF(SET_STR, mail_plugins),
	DEF(SET_STR, mail_plugin_dir),
	DEF(SET_STR, doveadm_socket_path),
	DEF(SET_UINT, doveadm_worker_count),
	DEF(SET_UINT, doveadm_port),
	{ SET_ALIAS, "doveadm_proxy_port", 0, NULL },
	DEF(SET_STR, doveadm_password),
	DEF(SET_STR, doveadm_allowed_commands),
	DEF(SET_STR, dsync_alt_char),
	DEF(SET_STR, dsync_remote_cmd),
	DEF(SET_STR, ssl_client_ca_dir),
	DEF(SET_STR, ssl_client_ca_file),

	{ SET_STRLIST, "plugin", offsetof(struct doveadm_settings, plugin_envs), NULL },

	SETTING_DEFINE_LIST_END
};
const struct doveadm_settings doveadm_default_settings = {
	.base_dir = PKG_RUNDIR,
	.libexec_dir = PKG_LIBEXECDIR,
	.mail_plugins = "",
	.mail_plugin_dir = MODULEDIR,
	.doveadm_socket_path = "doveadm-server",
	.doveadm_worker_count = 0,
	.doveadm_port = 0,
	.doveadm_password = "",
	.doveadm_allowed_commands = "",
	.dsync_alt_char = "_",
	.dsync_remote_cmd = "ssh -l%{login} %{host} doveadm dsync-server -u%u -U",
	.ssl_client_ca_dir = "",
	.ssl_client_ca_file = "",

	.plugin_envs = ARRAY_INIT
};
static const struct setting_parser_info *doveadm_setting_dependencies[] = {
	&mail_user_setting_parser_info,
	NULL
};
const struct setting_parser_info doveadm_setting_parser_info = {
	.module_name = "doveadm",
	.defines = doveadm_setting_defines,
	.defaults = &doveadm_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct doveadm_settings),

	.parent_offset = (size_t)-1,
	.check_func = doveadm_settings_check,
	.dependencies = doveadm_setting_dependencies
};
/* ../../src/dns/dns-client-settings.c */
/* <settings checks> */
static struct file_listener_settings dns_client_unix_listeners_array[] = {
	{ "dns-client", 0666, "", "" }
};
static struct file_listener_settings *dns_client_unix_listeners[] = {
	&dns_client_unix_listeners_array[0]
};
static buffer_t dns_client_unix_listeners_buf = {
	dns_client_unix_listeners, sizeof(dns_client_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings dns_client_service_settings = {
	.name = "dns_client",
	.protocol = "",
	.type = "",
	.executable = "dns-client",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &dns_client_unix_listeners_buf,
			      sizeof(dns_client_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
/* ../../src/director/director-settings.c */
/* <settings checks> */
static bool director_settings_verify(void *_set, pool_t pool, const char **error_r);

static struct file_listener_settings director_unix_listeners_array[] = {
	{ "login/director", 0, "", "" },
	{ "director-admin", 0600, "", "" }
};
static struct file_listener_settings *director_unix_listeners[] = {
	&director_unix_listeners_array[0],
	&director_unix_listeners_array[1]
};
static buffer_t director_unix_listeners_buf = {
	director_unix_listeners,
	sizeof(director_unix_listeners), { 0, }
};
static struct file_listener_settings director_fifo_listeners_array[] = {
	{ "login/proxy-notify", 0, "", "" }
};
static struct file_listener_settings *director_fifo_listeners[] = {
	&director_fifo_listeners_array[0]
};
static buffer_t director_fifo_listeners_buf = {
	director_fifo_listeners,
	sizeof(director_fifo_listeners), { 0, }
};
/* </settings checks> */
/* <settings checks> */
static bool
director_settings_verify(void *_set, pool_t pool ATTR_UNUSED, const char **error_r)
{
	struct director_settings *set = _set;

	if (set->director_user_expire < 10) {
		*error_r = "director_user_expire is too low";
		return FALSE;
	}
	return TRUE;
}
/* </settings checks> */
struct service_settings director_service_settings = {
	.name = "director",
	.protocol = "",
	.type = "",
	.executable = "director",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = ".",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = UINT_MAX,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &director_unix_listeners_buf,
			      sizeof(director_unix_listeners[0]) } },
	.fifo_listeners = { { &director_fifo_listeners_buf,
			      sizeof(director_fifo_listeners[0]) } },
	.inet_listeners = ARRAY_INIT,

	.process_limit_1 = TRUE
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct director_settings, name), NULL }
static const struct setting_define director_setting_defines[] = {
	DEF(SET_STR, master_user_separator),

	DEF(SET_STR, director_servers),
	DEF(SET_STR, director_mail_servers),
	DEF(SET_STR, director_username_hash),
	DEF(SET_TIME, director_user_expire),
	DEF(SET_UINT, director_doveadm_port),

	SETTING_DEFINE_LIST_END
};
const struct director_settings director_default_settings = {
	.master_user_separator = "",

	.director_servers = "",
	.director_mail_servers = "",
	.director_username_hash = "%Lu",
	.director_user_expire = 60*15,
	.director_doveadm_port = 0
};
const struct setting_parser_info director_setting_parser_info = {
	.module_name = "director",
	.defines = director_setting_defines,
	.defaults = &director_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct director_settings),

	.parent_offset = (size_t)-1,

	.check_func = director_settings_verify
};
/* ../../src/dict/dict-settings.c */
/* <settings checks> */
static struct file_listener_settings dict_unix_listeners_array[] = {
	{ "dict", 0600, "", "" }
};
static struct file_listener_settings *dict_unix_listeners[] = {
	&dict_unix_listeners_array[0]
};
static buffer_t dict_unix_listeners_buf = {
	dict_unix_listeners, sizeof(dict_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings dict_service_settings = {
	.name = "dict",
	.protocol = "",
	.type = "",
	.executable = "dict",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &dict_unix_listeners_buf,
			      sizeof(dict_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct dict_settings, name), NULL }
static const struct setting_define dict_setting_defines[] = {
	DEF(SET_STR, base_dir),
	DEF(SET_STR, dict_db_config),
	{ SET_STRLIST, "dict", offsetof(struct dict_settings, dicts), NULL },

	SETTING_DEFINE_LIST_END
};
const struct dict_settings dict_default_settings = {
	.base_dir = PKG_RUNDIR,
	.dict_db_config = "",
	.dicts = ARRAY_INIT
};
const struct setting_parser_info dict_setting_parser_info = {
	.module_name = "dict",
	.defines = dict_setting_defines,
	.defaults = &dict_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct dict_settings),

	.parent_offset = (size_t)-1
};
/* ../../src/config/config-settings.c */
/* <settings checks> */
static struct file_listener_settings config_unix_listeners_array[] = {
	{ "config", 0600, "", "" }
};
static struct file_listener_settings *config_unix_listeners[] = {
	&config_unix_listeners_array[0]
};
static buffer_t config_unix_listeners_buf = {
	config_unix_listeners, sizeof(config_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings config_service_settings = {
	.name = "config",
	.protocol = "",
	.type = "config",
	.executable = "config",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &config_unix_listeners_buf,
			      sizeof(config_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
/* ../../src/auth/auth-settings.c */
extern const struct setting_parser_info auth_passdb_setting_parser_info;
extern const struct setting_parser_info auth_userdb_setting_parser_info;
/* <settings checks> */
static struct file_listener_settings auth_unix_listeners_array[] = {
	{ "login/login", 0666, "", "" },
	{ "token-login/tokenlogin", 0666, "", "" },
	{ "auth-login", 0600, "$default_internal_user", "" },
	{ "auth-client", 0600, "", "" },
	{ "auth-userdb", 0666, "$default_internal_user", "" },
	{ "auth-master", 0600, "", "" }
};
static struct file_listener_settings *auth_unix_listeners[] = {
	&auth_unix_listeners_array[0],
	&auth_unix_listeners_array[1],
	&auth_unix_listeners_array[2],
	&auth_unix_listeners_array[3],
	&auth_unix_listeners_array[4],
	&auth_unix_listeners_array[5]
};
static buffer_t auth_unix_listeners_buf = {
	auth_unix_listeners, sizeof(auth_unix_listeners), { 0, }
};
/* </settings checks> */
/* <settings checks> */
static struct file_listener_settings auth_worker_unix_listeners_array[] = {
	{ "auth-worker", 0600, "$default_internal_user", "" }
};
static struct file_listener_settings *auth_worker_unix_listeners[] = {
	&auth_worker_unix_listeners_array[0]
};
static buffer_t auth_worker_unix_listeners_buf = {
	auth_worker_unix_listeners, sizeof(auth_worker_unix_listeners), { 0, }
};
/* </settings checks> */
/* <settings checks> */
static bool
auth_settings_set_self_ips(struct auth_settings *set, pool_t pool,
			   const char **error_r)
{
	const char *const *tmp;
	ARRAY(struct ip_addr) ips_array;
	struct ip_addr *ips;
	unsigned int ips_count;
	int ret;

	if (*set->proxy_self == '\0') {
		set->proxy_self_ips = p_new(pool, struct ip_addr, 1);
		return TRUE;
	}

	p_array_init(&ips_array, pool, 4);
	tmp = t_strsplit_spaces(set->proxy_self, " ");
	for (; *tmp != NULL; tmp++) {
		ret = net_gethostbyname(*tmp, &ips, &ips_count);
		if (ret != 0) {
			*error_r = t_strdup_printf("auth_proxy_self_ips: "
				"gethostbyname(%s) failed: %s",
				*tmp, net_gethosterror(ret));
		}
		array_append(&ips_array, ips, ips_count);
	}
	array_append_zero(&ips_array);
	set->proxy_self_ips = array_idx(&ips_array, 0);
	return TRUE;
}

static bool auth_settings_check(void *_set, pool_t pool,
				const char **error_r)
{
	struct auth_settings *set = _set;
	const char *p;

	if (set->debug_passwords)
		set->debug = TRUE;
	if (set->debug)
		set->verbose = TRUE;

	if (set->worker_max_count == 0) {
		*error_r = "auth_worker_max_count must be above zero";
		return FALSE;
	}

	if (set->cache_size > 0 && set->cache_size < 1024) {
		/* probably a configuration error.
		   older versions used megabyte numbers */
		*error_r = t_strdup_printf("auth_cache_size value is too small "
					   "(%"PRIuUOFF_T" bytes)",
					   set->cache_size);
		return FALSE;
	}

	if (*set->username_chars == '\0') {
		/* all chars are allowed */
		memset(set->username_chars_map, 1,
		       sizeof(set->username_chars_map));
	} else {
		for (p = set->username_chars; *p != '\0'; p++)
			set->username_chars_map[(int)(uint8_t)*p] = 1;
	}

	if (*set->username_translation != '\0') {
		p = set->username_translation;
		for (; *p != '\0' && p[1] != '\0'; p += 2)
			set->username_translation_map[(int)(uint8_t)*p] = p[1];
	}
	set->realms_arr =
		(const char *const *)p_strsplit_spaces(pool, set->realms, " ");

	if (!auth_settings_set_self_ips(set, pool, error_r))
		return FALSE;
	return TRUE;
}

static bool
auth_passdb_settings_check(void *_set, pool_t pool ATTR_UNUSED,
			   const char **error_r)
{
	struct auth_passdb_settings *set = _set;

	if (set->driver == NULL || *set->driver == '\0') {
		*error_r = "passdb is missing driver";
		return FALSE;
	}
	if (set->pass && strcmp(set->result_success, "return-ok") != 0) {
		*error_r = "Obsolete pass=yes setting mixed with non-default result_success";
		return FALSE;
	}
	return TRUE;
}

static bool
auth_userdb_settings_check(void *_set, pool_t pool ATTR_UNUSED,
			   const char **error_r)
{
	struct auth_userdb_settings *set = _set;

	if (set->driver == NULL || *set->driver == '\0') {
		*error_r = "userdb is missing driver";
		return FALSE;
	}
	return TRUE;
}
/* </settings checks> */
struct service_settings auth_service_settings = {
	.name = "auth",
	.protocol = "",
	.type = "",
	.executable = "auth",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &auth_unix_listeners_buf,
			      sizeof(auth_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT,

	.process_limit_1 = TRUE
};
struct service_settings auth_worker_service_settings = {
	.name = "auth-worker",
	.protocol = "",
	.type = "",
	.executable = "auth -w",
	.user = "",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 1,
	.idle_kill = 0,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &auth_worker_unix_listeners_buf,
			      sizeof(auth_worker_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct auth_passdb_settings, name), NULL }
static const struct setting_define auth_passdb_setting_defines[] = {
	DEF(SET_STR, driver),
	DEF(SET_STR, args),
	DEF(SET_STR, default_fields),
	DEF(SET_STR, override_fields),
	DEF(SET_ENUM, skip),
	DEF(SET_ENUM, result_success),
	DEF(SET_ENUM, result_failure),
	DEF(SET_ENUM, result_internalfail),
	DEF(SET_BOOL, deny),
	DEF(SET_BOOL, pass),
	DEF(SET_BOOL, master),

	SETTING_DEFINE_LIST_END
};
static const struct auth_passdb_settings auth_passdb_default_settings = {
	.driver = "",
	.args = "",
	.default_fields = "",
	.override_fields = "",
	.skip = "never:authenticated:unauthenticated",
	.result_success = "return-ok:return:return-fail:continue:continue-ok:continue-fail",
	.result_failure = "continue:return:return-ok:return-fail:continue-ok:continue-fail",
	.result_internalfail = "continue:return:return-ok:return-fail:continue-ok:continue-fail",
	.deny = FALSE,
	.pass = FALSE,
	.master = FALSE
};
const struct setting_parser_info auth_passdb_setting_parser_info = {
	.defines = auth_passdb_setting_defines,
	.defaults = &auth_passdb_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct auth_passdb_settings),

	.parent_offset = (size_t)-1,
	.parent = &auth_setting_parser_info,

	.check_func = auth_passdb_settings_check
};
#undef DEF
#define DEF(type, name) \
	{ type, #name, offsetof(struct auth_userdb_settings, name), NULL }
static const struct setting_define auth_userdb_setting_defines[] = {
	DEF(SET_STR, driver),
	DEF(SET_STR, args),
	DEF(SET_STR, default_fields),
	DEF(SET_STR, override_fields),

	SETTING_DEFINE_LIST_END
};
static const struct auth_userdb_settings auth_userdb_default_settings = {
	.driver = "",
	.args = "",
	.default_fields = "",
	.override_fields = ""
};
const struct setting_parser_info auth_userdb_setting_parser_info = {
	.defines = auth_userdb_setting_defines,
	.defaults = &auth_userdb_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct auth_userdb_settings),

	.parent_offset = (size_t)-1,
	.parent = &auth_setting_parser_info,

	.check_func = auth_userdb_settings_check
};
#undef DEF
#undef DEF_NOPREFIX
#undef DEFLIST
#define DEF(type, name) \
	{ type, "auth_"#name, offsetof(struct auth_settings, name), NULL }
#define DEF_NOPREFIX(type, name) \
	{ type, #name, offsetof(struct auth_settings, name), NULL }
#define DEFLIST(field, name, defines) \
	{ SET_DEFLIST, name, offsetof(struct auth_settings, field), defines }
static const struct setting_define auth_setting_defines[] = {
	DEF(SET_STR, mechanisms),
	DEF(SET_STR, realms),
	DEF(SET_STR, default_realm),
	DEF(SET_SIZE, cache_size),
	DEF(SET_TIME, cache_ttl),
	DEF(SET_TIME, cache_negative_ttl),
	DEF(SET_STR, username_chars),
	DEF(SET_STR, username_translation),
	DEF(SET_STR, username_format),
	DEF(SET_STR, master_user_separator),
	DEF(SET_STR, anonymous_username),
	DEF(SET_STR, krb5_keytab),
	DEF(SET_STR, gssapi_hostname),
	DEF(SET_STR, winbind_helper_path),
	DEF(SET_STR, proxy_self),
	DEF(SET_TIME, failure_delay),

	DEF(SET_BOOL, verbose),
	DEF(SET_BOOL, debug),
	DEF(SET_BOOL, debug_passwords),
	DEF(SET_ENUM, verbose_passwords),
	DEF(SET_BOOL, ssl_require_client_cert),
	DEF(SET_BOOL, ssl_username_from_cert),
	DEF(SET_BOOL, use_winbind),

	DEF(SET_UINT, worker_max_count),

	DEFLIST(passdbs, "passdb", &auth_passdb_setting_parser_info),
	DEFLIST(userdbs, "userdb", &auth_userdb_setting_parser_info),

#ifdef APPLE_OS_X_SERVER
	DEF_NOPREFIX(SET_STR, aps_topic),
#endif
	DEF_NOPREFIX(SET_STR, base_dir),
	DEF_NOPREFIX(SET_BOOL, verbose_proctitle),
	DEF_NOPREFIX(SET_UINT, first_valid_uid),
	DEF_NOPREFIX(SET_UINT, last_valid_uid),

	SETTING_DEFINE_LIST_END
};
static const struct auth_settings auth_default_settings = {
	.mechanisms = "plain",
	.realms = "",
	.default_realm = "",
	.cache_size = 0,
	.cache_ttl = 60*60,
	.cache_negative_ttl = 60*60,
	.username_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890.-_@",
	.username_translation = "",
	.username_format = "%Lu",
	.master_user_separator = "",
	.anonymous_username = "anonymous",
	.krb5_keytab = "",
	.gssapi_hostname = "",
	.winbind_helper_path = "/usr/bin/ntlm_auth",
	.proxy_self = "",
	.failure_delay = 2,

	.verbose = FALSE,
	.debug = FALSE,
	.debug_passwords = FALSE,
	.verbose_passwords = "no:plain:sha1",
	.ssl_require_client_cert = FALSE,
	.ssl_username_from_cert = FALSE,
	.use_winbind = FALSE,

	.worker_max_count = 30,

	.passdbs = ARRAY_INIT,
	.userdbs = ARRAY_INIT,

#ifdef APPLE_OS_X_SERVER
	.aps_topic = "",
#endif
	.base_dir = PKG_RUNDIR,
	.verbose_proctitle = FALSE,
	.first_valid_uid = 500,
	.last_valid_uid = 0,
};
const struct setting_parser_info auth_setting_parser_info = {
	.module_name = "auth",
	.defines = auth_setting_defines,
	.defaults = &auth_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct auth_settings),

	.parent_offset = (size_t)-1,

	.check_func = auth_settings_check
};
/* ../../src/anvil/anvil-settings.c */
/* <settings checks> */
static struct file_listener_settings anvil_unix_listeners_array[] = {
	{ "anvil", 0600, "", "" },
	{ "anvil-auth-penalty", 0600, "", "" }
};
static struct file_listener_settings *anvil_unix_listeners[] = {
	&anvil_unix_listeners_array[0],
	&anvil_unix_listeners_array[1]
};
static buffer_t anvil_unix_listeners_buf = {
	anvil_unix_listeners, sizeof(anvil_unix_listeners), { 0, }
};
/* </settings checks> */
struct service_settings anvil_service_settings = {
	.name = "anvil",
	.protocol = "",
	.type = "anvil",
	.executable = "anvil",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "empty",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 1,
	.process_limit = 1,
	.client_limit = 0,
	.service_count = 0,
	.idle_kill = UINT_MAX,
	.vsz_limit = (uoff_t)-1,

	.unix_listeners = { { &anvil_unix_listeners_buf,
			      sizeof(anvil_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT,

	.process_limit_1 = TRUE
};
static struct service_settings *config_all_services[] = {
#ifdef HAVE_LIBWRAP
	&tcpwrap_service_settings,
#endif
	&stats_service_settings,
	&ssl_params_service_settings,
	&replicator_service_settings,
	&aggregator_service_settings,
	&pop3_service_settings,
	&pop3_login_service_settings,
	&log_service_settings,
	&lmtp_service_settings,
	&ipc_service_settings,
	&indexer_worker_service_settings,
	&indexer_service_settings,
	&imap_service_settings,
	&imap_urlauth_worker_service_settings,
	&imap_urlauth_service_settings,
	&imap_urlauth_login_service_settings,
	&imap_login_service_settings,
	&doveadm_service_settings,
	&dns_client_service_settings,
	&director_service_settings,
	&dict_service_settings,
	&config_service_settings,
	&auth_service_settings,
	&auth_worker_service_settings,
	&anvil_service_settings,
};
buffer_t config_all_services_buf = {
	config_all_services, sizeof(config_all_services), { 0, }
};
const struct setting_parser_info *all_default_roots[] = {
	&master_service_setting_parser_info,
	&master_service_ssl_setting_parser_info,
	&dict_setting_parser_info, 
	&master_setting_parser_info, 
	&pop3_login_setting_parser_info, 
	&pop3_setting_parser_info, 
	&imap_urlauth_login_setting_parser_info, 
	&replicator_setting_parser_info, 
	&lda_setting_parser_info, 
	&imap_urlauth_setting_parser_info, 
	&aggregator_setting_parser_info, 
	&ssl_params_setting_parser_info, 
	&mdbox_setting_parser_info, 
	&doveadm_setting_parser_info, 
	&mail_user_setting_parser_info, 
	&imap_login_setting_parser_info, 
	&mail_storage_setting_parser_info, 
	&director_setting_parser_info, 
	&stats_setting_parser_info, 
	&imapc_setting_parser_info, 
	&maildir_setting_parser_info, 
	&lmtp_setting_parser_info, 
	&imap_urlauth_worker_setting_parser_info, 
	&login_setting_parser_info, 
	&auth_setting_parser_info, 
	&mbox_setting_parser_info, 
	&pop3c_setting_parser_info, 
	&imap_setting_parser_info, 
	NULL
};
const struct setting_parser_info *const *all_roots = all_default_roots;
ARRAY_TYPE(service_settings) *default_services = &master_default_settings.services;
