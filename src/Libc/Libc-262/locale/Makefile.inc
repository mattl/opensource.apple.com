#	from @(#)Makefile.inc	8.1 (Berkeley) 6/4/93
# $FreeBSD: src/lib/libc/locale/Makefile.inc,v 1.20 2001/05/03 15:12:52 phantom Exp $

# locale sources
.PATH: ${.CURDIR}/${MACHINE_ARCH}/locale ${.CURDIR}/locale

SRCS+=  ansi.c big5.c collate.c collcmp.c euc.c frune.c isctype.c \
	lconv.c localeconv.c mbrune.c mskanji.c nomacros.c none.c rune.c \
	runetype.c setinvalidrune.c setlocale.c setrunelocale.c table.c \
	tolower.c toupper.c utf2.c

.if ${LIB} == "c"
MAN3+=	ctype.3 isalnum.3 isalpha.3 isascii.3 isblank.3 iscntrl.3 \
	isdigit.3 isgraph.3 islower.3 isprint.3 ispunct.3 isspace.3 \
	isupper.3 isxdigit.3 mbrune.3 multibyte.3 nl_langinfo.3 rune.3 \
	setlocale.3 toascii.3 tolower.3 toupper.3
MAN4+=	euc.4 utf2.4

MLINKS+=mbrune.3 mbmb.3 mbrune.3 mbrrune.3
MLINKS+=multibyte.3 mblen.3 multibyte.3 mbstowcs.3 multibyte.3 mbtowc.3 \
	multibyte.3 wcstombs.3 multibyte.3 wctomb.3
MLINKS+=rune.3 fgetrune.3 rune.3 fputrune.3 rune.3 fungetrune.3 \
	rune.3 setinvalidrune.3 rune.3 setrunelocale.3 rune.3 sgetrune.3 \
	rune.3 sputrune.3
MLINKS+=setlocale.3 localeconv.3
.endif
