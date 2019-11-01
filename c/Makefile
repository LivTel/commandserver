# $Header: /home/cjm/cvs/commandserver/c/Makefile,v 1.10 2019-11-01 16:28:52 cjm Exp $

include ../../Makefile.common
include ../Makefile.common

BINDIR			= $(COMMANDSERVER_BIN_HOME)/c/$(HOSTTYPE)
INCDIR 			= $(COMMANDSERVER_SRC_HOME)/include
DOCSDIR 		= $(COMMANDSERVER_DOC_HOME)/cdocs

# log_udp library (log_udp.h is included for verbosity settings)
LOG_UDP_CFLAGS		= -I$(LOG_UDP_SRC_HOME)/include

#DEBUG_CFLAGS	= 
DEBUG_CFLAGS	= -DCOMMAND_SERVER_DEBUG=6

CFLAGS 		= -g -I$(INCDIR) $(DEBUG_CFLAGS) $(LOG_UDP_CFLAGS) $(SHARED_LIB_CFLAGS)
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
	makedepend $(MAKEDEPENDFLAGS) -p$(BINDIR)/ -- $(CFLAGS) -- $(SRCS)

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

#
# $Log: not supported by cvs2svn $
# Revision 1.9  2012/02/20 10:59:41  cjm
# makedepend
#
# Revision 1.8  2010/02/09 10:42:25  cjm
# *** empty log message ***
#
# Revision 1.7  2009/01/30 15:40:26  cjm
# Added log_udp include file.
#
# Revision 1.6  2006/10/19 10:15:07  cjm
# Changed COMMAND_SERVER_DEBUG to be integer based to give more granular control of logging.
#
#

# DO NOT DELETE

/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/arpa/inet.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/features.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sys/cdefs.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/wordsize.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/gnu/stubs.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/gnu/stubs-32.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/netinet/in.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/stdint.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/wchar.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sys/socket.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sys/uio.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sys/types.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/types.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/lib/gcc/i386-redhat-linux/4.1.1/include/stddef.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/typesizes.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/time.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/endian.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/endian.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sys/select.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/select.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/sigset.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/time.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sys/sysmacros.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/pthreadtypes.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/uio.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/socket.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/lib/gcc/i386-redhat-linux/4.1.1/include/limits.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/lib/gcc/i386-redhat-linux/4.1.1/include/syslimits.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/limits.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/posix1_lim.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/local_lim.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/linux/limits.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/posix2_lim.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/xopen_lim.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/stdio_lim.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/sockaddr.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/asm/socket.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/asm/sockios.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/in.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/byteswap.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/netinet/tcp.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/errno.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/errno.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/linux/errno.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/asm/errno.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/asm-generic/errno.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/asm-generic/errno-base.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/netdb.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/rpc/netdb.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/netdb.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/pthread.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/sched.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/sched.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/signal.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/setjmp.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/lib/gcc/i386-redhat-linux/4.1.1/include/stdarg.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/stdlib.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/waitflags.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/waitstatus.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/alloca.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/stdio.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/libio.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/_G_config.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/wchar.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/gconv.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/sys_errlist.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/string.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/unistd.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/posix_opt.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/environments.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/bits/confname.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /usr/include/getopt.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /home/dev/src/log_udp/include/log_udp.h
/home/dev/bin/commandserver/c/i386-linux/command_server.o: /home/dev/src/commandserver/include/command_server.h
