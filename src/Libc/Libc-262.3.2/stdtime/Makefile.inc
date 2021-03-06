#	Makefile.inc,v 1.2 1994/09/13 21:26:01 wollman Exp
# $FreeBSD: src/lib/libc/stdtime/Makefile.inc,v 1.13 2001/10/28 19:54:49 dillon Exp $

.PATH:	${.CURDIR}/stdtime ${.CURDIR}/locale

SRCS+=	timelocal.c asctime.c difftime.c localtime.c strftime.c strptime.c

.if ${LIB} == "c"
MAN3+=	ctime.3 strftime.3 strptime.3 time2posix.3
MAN5+=	tzfile.5

MLINKS+=ctime.3 asctime.3 ctime.3 difftime.3 ctime.3 gmtime.3 \
	ctime.3 localtime.3 ctime.3 mktime.3 ctime.3 timegm.3 \
	ctime.3 ctime_r.3 ctime.3 localtime_r.3 ctime.3 gmtime_r.3 \
	ctime.3 asctime_r.3
MLINKS+=time2posix.3 posix2time.3
.endif
