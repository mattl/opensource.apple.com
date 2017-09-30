if BUILD_ZLIB_PLUGIN
ZLIB = zlib imap-zlib
endif

if BUILD_PUSH_NOTIFY
PUSH_NOTIFY = push-notify
endif

if BUILD_LUCENE
FTS_LUCENE = fts-lucene
endif

if BUILD_SK
FTS_SK = fts-sk
endif

if BUILD_SOLR
FTS_SOLR = fts-solr
endif

SUBDIRS = \
	acl \
	imap-acl \
	autocreate \
	expire \
	fts \
	fts-squat \
	lazy-expunge \
	listescape \
	notify \
	mail-log \
	mailbox-alias \
	$(PUSH_NOTIFY) \
	quota \
	imap-quota \
	pop3-migration \
	replication \
	snarf \
	stats \
	imap-stats \
	trash \
	virtual \
	$(ZLIB) \
	$(FTS_LUCENE) \
	$(FTS_SK) \
	$(FTS_SOLR)
