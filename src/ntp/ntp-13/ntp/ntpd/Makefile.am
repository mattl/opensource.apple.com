#AUTOMAKE_OPTIONS = ../util/ansi2knr no-dependencies
AUTOMAKE_OPTIONS = ../util/ansi2knr
sbin_PROGRAMS = ntpd
INCLUDES = -I$(top_srcdir)/include
LIBMATH = -lm
# LDADD might need RESLIB and ADJLIB
LDADD =	version.o @LIBPARSE@ ../libntp/libntp.a @LIBRSAREF@
ntpd_LDADD = $(LDADD) $(LIBMATH)
DISTCLEANFILES = .version version.c
#EXTRA_DIST = ntpd.mak
ETAGS_ARGS = Makefile.am

ntpd_SOURCES = map_vme.c ntp_config.c ntp_control.c ntp_io.c \
	ntp_loopfilter.c ntp_monitor.c ntp_peer.c ntp_proto.c \
	ntp_refclock.c ntp_request.c ntp_restrict.c ntp_timer.c \
	ntp_util.c ntp_intres.c ntp_filegen.c ntpd.c \
	refclock_conf.c refclock_chu.c refclock_local.c \
	refclock_pst.c refclock_wwvb.c refclock_mx4200.c \
	refclock_parse.c refclock_as2201.c refclock_bancomm.c \
	refclock_tpro.c refclock_leitch.c refclock_irig.c \
	refclock_msfees.c refclock_trak.c refclock_datum.c \
	refclock_acts.c refclock_heath.c refclock_nmea.c \
	refclock_atom.c refclock_ptbacts.c refclock_jupiter.c \
	refclock_usno.c refclock_true.c refclock_hpgps.c \
	refclock_shm.c refclock_gpsvme.c refclock_arbiter.c \
	refclock_arc.c refclock_palisade.c refclock_palisade.h \
	refclock_oncore.c refclock_chronolog.c refclock_dumbclock.c \
	jupiter.h

$(PROGRAMS): $(LDADD)

../libntp/libntp.a:
	cd ../libntp && $(MAKE)

../libparse/libparse.a:
	cd ../libparse && $(MAKE)

version.o: $(ntpd_OBJECTS) ../libntp/libntp.a @LIBPARSE@ @LIBRSAREF@ Makefile
	$(top_builddir)/scripts/mkver ntpd
	$(COMPILE) -c version.c
