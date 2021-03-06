RC_OS = macos
OFLAG = -O
ifeq "macos" "$(RC_OS)"
  X_CFLAGS := $(shell if [ "$(RC_RELEASE)" != "Beaker" ] &&    \
			 [ "$(RC_RELEASE)" != "Bunsen" ] &&    \
			 [ "$(RC_RELEASE)" != "Gonzo"  ] &&    \
			 [ "$(RC_RELEASE)" != "Kodiak" ]; then \
	     echo -Wno-long-double -no-cpp-precomp ; \
	     fi; )
endif


USE_DEPENDENCY_FILE := $(shell if [ "$(notdir $(CC))" != "mwccppc" ]; then \
	     echo YES ; else echo NO ; \
	     fi; )
	
ifeq "mwccppc" "$(notdir $(CC))"
  CFLAGS = $(OFLAG) -g -gccinc -I$(SRCROOT)/../include
else
  CFLAGS = $(OFLAG) -g -I$(SRCROOT)/../include -Wall $(X_CFLAGS)
endif

ifneq "" "$(wildcard /bin/mkdirs)"
  MKDIRS = /bin/mkdirs
else
  MKDIRS = /bin/mkdir -p
endif

SRCROOT = .
SYMROOT = .
OBJROOT = .
OFILE_DIR = $(OBJROOT)
VPATH = $(OFILE_DIR)

CFILES = libsyminfo.c
OBJS = $(CFILES:.c=.o)
INSTALL_FILES = $(CFILES) Makefile notes
LOCLIBDIR = /usr/local/lib
LIBSTUFF = -L$(SYMROOT)/../libstuff -lstuff

all: $(OFILE_DIR) $(SYMROOT) lib_ofiles

lib_ofiles: $(OFILE_DIR) $(SYMROOT) libsyminfo.a 

ifeq "NO" "$(USE_DEPENDENCY_FILE)"
.c.o:
	$(CC) $(CFLAGS) $(RC_CFLAGS) $(COPTS) -c -o $(OFILE_DIR)/$*.o $<
else
.c.o:
	$(CC) $(CFLAGS) $(RC_CFLAGS) $(COPTS) -c \
		-MD -dependency-file $(OFILE_DIR)/$*.d \
		-o $(OFILE_DIR)/$*.o $<
	md -d -u $(OFILE_DIR)/Makedep $(OFILE_DIR)/$*.d
endif

clean shlib_clean:
	-rm -r -f $(OFILE_DIRS)
	-rm -f libsyminfo.a

install: all

installsrc:
	$(MKDIRS) $(SRCROOT)
	chmod 755 $(SRCROOT)
	gnutar cf - $(INSTALL_FILES) | (cd $(SRCROOT); gnutar xf -)
	chmod 444 $(SRCROOT)/*

$(SYMROOT):
	$(MKDIRS) $@

libsyminfo.o: libsyminfo.c
ifeq "NO" "$(USE_DEPENDENCY_FILE)"
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c -static \
		-o $(OFILE_DIR)/$*.o $(SRCROOT)/libsyminfo.c -DLIBRARY_API
else
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c \
		-MD -dependency-file $(OFILE_DIR)/$*.d -o $(OFILE_DIR)/$*.o \
		$(SRCROOT)/libsyminfo.c -DLIBRARY_API
	md -d -u $(OFILE_DIR)/Makedep $(OFILE_DIR)/$*.d
endif

libsyminfo.a: libsyminfo.o
	$(CC) $(CFLAGS) $(RC_CFLAGS) -nostdlib -r \
		-o $(OBJROOT)/libsyminfo.o \
		libsyminfo.o $(LIBSTUFF)
	libtool -static -o $(SYMROOT)/libsyminfo.a \
		$(OBJROOT)/libsyminfo.o


lib_ofiles_install: lib_ofiles
	$(MKDIRS) $(DSTROOT)$(LOCLIBDIR)
	install -p -c -m 444 $(SYMROOT)/libsyminfo.a \
		$(DSTROOT)$(LOCLIBDIR)/libsyminfo.a
	ranlib $(DSTROOT)$(LOCLIBDIR)/libsyminfo.a

-include $(OFILE_DIR)/Makedep

$(OFILE_DIR) $(SYMROOT):
	$(MKDIRS) $@
