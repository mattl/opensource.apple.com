SHELL = /bin/sh

RC_CFLAGS ?= -arch ppc -arch i386

CC     ?= gcc
CXX    ?= g++
CFLAGS ?= -Wall -g -Os
nolink = -c

LD      ?= ld
LDFLAGS ?= -dynamic -arch_multiple

RM      = rm
RMFLAGS = -f

MKDIR      = mkdir
MKDIRFLAGS =

CP      = cp
CPFLAGS = -r

STRIP      = strip
STRIPFLAGS =

INSTALL      = install
INSTALLFLAGS =

SRCROOT ?= .
DSTROOT ?= .
OBJROOT ?= .

$(OBJROOT)/%.o: %.s
	$(CC) $(nolink) $(CFLAGS) $(RC_CFLAGS) -o $@ $<

$(OBJROOT)/%.o: %.c
	$(CC) $(nolink) $(CFLAGS) $(RC_CFLAGS) -o $@ $<

all: update_prebinding

$(OBJROOT)/dstart.o: start.s
	$(CC) $(nolink) $(CFLAGS) $(RC_CFLAGS) -dynamic -DCRT1 -o $@ $^

$(OBJROOT)/dyld.o: dyld.s
	$(CC) $(nolink) $(CFLAGS) $(RC_CFLAGS) -dynamic -DCRT1 -o $@ $^

$(OBJROOT)/ddyld.o: $(OBJROOT)/dyld.o /usr/lib/dyld
	$(CC) -r -nostdlib -keep_private_externs $(RC_CFLAGS) -o $@ $^

update_prebinding_core: $(OBJROOT)/dstart.o $(OBJROOT)/ddyld.o $(OBJROOT)/update_prebinding_core.o
	$(CC) -nostdlib $(RC_CFLAGS) -o $@  $^

update_prebinding : update_prebinding.cxx update_prebinding_core
	$(CXX) $(CFLAGS) $(RC_CFLAGS) update_prebinding.cxx -o $@


### B&I Makefile APIs
clean::
	$(RM) $(RMFLAGS) $(SRCROOT)/update_prebinding
	$(RM) $(RMFLAGS) $(SRCROOT)/update_prebinding_core
	$(RM) $(RMFLAGS) $(OBJROOT)/update_prebinding_core.o
	$(RM) $(RMFLAGS) $(OBJROOT)/{dstart,dyld,ddyld}.o
installhdrs::
install:: all installhdrs
	$(INSTALL) $(INSTALLFLAGS) -m 0755 -o root -g wheel -d $(DSTROOT)/usr/bin
	$(INSTALL) $(INSTALLFLAGS) -m 0755 -o root -g wheel -d $(DSTROOT)/usr/share/man/man1
	$(STRIP) $(STRIPFLAGS) update_prebinding
	$(STRIP) $(STRIPFLAGS) update_prebinding_core
	$(INSTALL) $(INSTALLFLAGS) -m 0755 -o root -g wheel update_prebinding $(DSTROOT)/usr/bin
	$(INSTALL) $(INSTALLFLAGS) -m 0755 -o root -g wheel update_prebinding_core $(DSTROOT)/usr/bin
	$(INSTALL) $(INSTALLFLAGS) -m 0644 -o root -g wheel man/man1/update_prebinding.1 $(DSTROOT)/usr/share/man/man1
	$(INSTALL) $(INSTALLFLAGS) -m 0644 -o root -g wheel man/man1/update_prebinding_core.1 $(DSTROOT)/usr/share/man/man1
	$(MAKE) clean
installsrc::
	$(MKDIR) $(MKDIRFLAGS) $(SRCROOT)/bin
	$(MKDIR) $(MKDIRFLAGS) $(SRCROOT)/man
	$(MKDIR) $(MKDIRFLAGS) $(SRCROOT)/man/man1
	$(CP) $(CPFLAGS) Makefile $(SRCROOT)
	$(CP) $(CPFLAGS) update_prebinding.cxx $(SRCROOT)
	$(CP) $(CPFLAGS) update_prebinding_core.c $(SRCROOT)
	$(CP) $(CPFLAGS) start.s $(SRCROOT)
	$(CP) $(CPFLAGS) dyld.s $(SRCROOT)
	$(CP) $(CPFLAGS) man/man1/update_prebinding.1 $(SRCROOT)/man/man1
	$(CP) $(CPFLAGS) man/man1/update_prebinding_core.1 $(SRCROOT)/man/man1
