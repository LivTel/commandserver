# $Header: /home/cjm/cvs/commandserver/c/Makefile,v 1.5 2006-05-26 10:06:53 cjm Exp $

include ../../../Makefile.common
include ../../Makefile.common
include ../Makefile.common

BINDIR			= $(AUTOGUIDER_COMMANDSERVER_BIN_HOME)/c/$(HOSTTYPE)
INCDIR 			= $(AUTOGUIDER_COMMANDSERVER_SRC_HOME)/include
DOCSDIR 		= $(AUTOGUIDER_COMMANDSERVER_DOC_HOME)/cdocs

#DEBUG_CFLAGS	= 
DEBUG_CFLAGS	= -DCOMMAND_SERVER_DEBUG

CFLAGS 		= -g -I$(INCDIR) $(DEBUG_CFLAGS)
DOCFLAGS 	= -static

LIB_SRCS	= command_server.c
SRCS		= $(LIB_SRCS)
LIB_HEADERS	= $(LIB_SRCS:%.c=$(INCDIR)/%.h)
HEADERS		= $(LIB_HEADERS)
LIB_OBJS	= $(LIB_SRCS:%.c=$(BINDIR)/%.o)
OBJS		= $(SRCS:%.c=$(BINDIR)/%.o)
DOCS 		= $(SRCS:%.c=$(DOCSDIR)/%.html)

top: $(LT_LIB_HOME)/$(LIBRARYNAME).so docs

static: $(LT_LIB_HOME)/$(LIBRARYNAME).so

$(LT_LIB_HOME)/$(LIBRARYNAME).so : $(LIB_OBJS)
	$(CC) $(CCSHAREDFLAG) $(CFLAGS) $(LIB_OBJS) -o $@

$(LT_LIB_HOME)/$(LIBRARYNAME).a : $(LIB_OBJS)
	ar rcv $@ $?

#$(BINDIR)/%: %.o
#	$(CC) $< -o $@  -L$(LT_LIB_HOME) $(TIMELIB) $(SOCKETLIB) -lm -lc

$(BINDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@  

docs: $(DOCS)

$(DOCS): $(SRCS)
	-$(CDOC) -d $(DOCSDIR) -h $(INCDIR) $(DOCFLAGS) $(SRCS)

$(DOCS) : $(SRCS)

depend:
	makedepend -p$(BINDIR)/ -- $(CFLAGS) -- $(SRCS)

clean:
	$(RM) $(RM_OPTIONS) $(EXES) $(OBJS) $(TIDY_OPTIONS)

tidy:
	$(RM) $(RM_OPTIONS) $(TIDY_OPTIONS)

backup: tidy
	$(RM) $(RM_OPTIONS) $(OBJS)

checkin:
	-$(CI) $(CI_OPTIONS) $(SRCS)
	-(cd $(INCDIR); $(CI) $(CI_OPTIONS) $(HEADERS);)

checkout:
	-$(CO) $(CO_OPTIONS) $(SRCS)
	-(cd $(INCDIR); $(CO) $(CO_OPTIONS) $(HEADERS);)

# DO NOT DELETE

command_server.o: /usr/include/arpa/inet.h /usr/include/features.h
command_server.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
command_server.o: /usr/include/netinet/in.h /usr/include/stdint.h
command_server.o: /usr/include/bits/wchar.h /usr/include/bits/wordsize.h
command_server.o: /usr/include/bits/types.h
command_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
command_server.o: /usr/include/bits/typesizes.h /usr/include/bits/socket.h
command_server.o: /usr/include/limits.h
command_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
command_server.o: /usr/include/bits/posix1_lim.h
command_server.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
command_server.o: /usr/include/bits/posix2_lim.h
command_server.o: /usr/include/bits/xopen_lim.h /usr/include/bits/stdio_lim.h
command_server.o: /usr/include/sys/types.h /usr/include/time.h
command_server.o: /usr/include/endian.h /usr/include/bits/endian.h
command_server.o: /usr/include/sys/select.h /usr/include/bits/select.h
command_server.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
command_server.o: /usr/include/sys/sysmacros.h
command_server.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
command_server.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
command_server.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
command_server.o: /usr/include/bits/byteswap.h /usr/include/sys/socket.h
command_server.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
command_server.o: /usr/include/errno.h /usr/include/bits/errno.h
command_server.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
command_server.o: /usr/include/netdb.h /usr/include/rpc/netdb.h
command_server.o: /usr/include/bits/netdb.h /usr/include/pthread.h
command_server.o: /usr/include/sched.h /usr/include/signal.h
command_server.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
command_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
command_server.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
command_server.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
command_server.o: /usr/include/stdio.h /usr/include/libio.h
command_server.o: /usr/include/_G_config.h /usr/include/wchar.h
command_server.o: /usr/include/gconv.h /usr/include/bits/sys_errlist.h
command_server.o: /usr/include/string.h /usr/include/unistd.h
command_server.o: /usr/include/bits/posix_opt.h
command_server.o: /usr/include/bits/environments.h
command_server.o: /usr/include/bits/confname.h /usr/include/getopt.h
command_server.o: /home/dev/src/autoguider/commandserver/include/command_server.h
