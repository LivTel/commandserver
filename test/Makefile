# $Header: /home/cjm/cvs/commandserver/test/Makefile,v 1.5 2009-01-30 15:41:03 cjm Exp $

include ../../../Makefile.common
include ../../Makefile.common
include ../Makefile.common

TEST_HOME		= test
BINDIR			= $(AUTOGUIDER_COMMANDSERVER_BIN_HOME)/$(TEST_HOME)/$(HOSTTYPE)
INCDIR 			= $(AUTOGUIDER_COMMANDSERVER_SRC_HOME)/include
DOCSDIR 		= $(AUTOGUIDER_COMMANDSERVER_DOC_HOME)/$(TEST_HOME)

# log_udp library (log_udp.h is included for verbosity settings)
LOG_UDP_CFLAGS		= -I$(LOG_UDP_SRC_HOME)/include

#DEBUG_CFLAGS	= 
DEBUG_CFLAGS	= -DCOMMAND_SERVER_DEBUG=5
#-DGETFITS_DEBUG
#-DGETFITS_DEBUG2

CFLAGS 		= -g -I$(INCDIR) -I$(CFITSIOINCDIR) $(DEBUG_CFLAGS) $(LOG_UDP_CFLAGS)
DOCFLAGS 	= -static

EXE_SRCS	= test_server.c send_command.c test_getfits_command.c
SRCS		= $(EXE_SRCS)
EXE_OBJS	= $(EXE_SRCS:%.c=$(BINDIR)/%.o)
OBJS		= $(SRCS:%.c=$(BINDIR)/%.o)
EXES		= $(EXE_SRCS:%.c=$(BINDIR)/%)
DOCS 		= $(SRCS:%.c=$(DOCSDIR)/%.html)

top: $(EXES) docs

$(BINDIR)/test_server: $(BINDIR)/test_server.o
	$(CC) $< -o $@  -L$(LT_LIB_HOME) -lautoguider_commandserver -lcfitsio $(TIMELIB) $(SOCKETLIB) -lm -lc

$(BINDIR)/%: $(BINDIR)/%.o
	$(CC) $< -o $@  -L$(LT_LIB_HOME) -lautoguider_commandserver $(TIMELIB) $(SOCKETLIB) -lm -lc

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

checkout:
	-$(CO) $(CO_OPTIONS) $(SRCS)

# DO NOT DELETE

/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/features.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/sys/cdefs.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/gnu/stubs.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/linux/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/asm/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/malloc.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/unistd.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/posix_opt.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/types.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/wordsize.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/typesizes.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/confname.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/getopt.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/stdio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/libio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/_G_config.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/wchar.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/wchar.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/gconv.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/stdio_lim.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/sys_errlist.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/stdlib.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/string.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /home/dev/src/cfitsio3.006/include/fitsio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/limits.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/posix1_lim.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/local_lim.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/linux/limits.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /usr/include/bits/posix2_lim.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /home/dev/src/cfitsio3.006/include/longnam.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_server.o: /home/dev/src/autoguider/commandserver/include/command_server.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/stdio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/features.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/sys/cdefs.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/gnu/stubs.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/types.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/wordsize.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/typesizes.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/libio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/_G_config.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/wchar.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/wchar.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/gconv.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/stdio_lim.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/sys_errlist.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/stdlib.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/string.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/unistd.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/posix_opt.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/bits/confname.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /usr/include/getopt.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/send_command.o: /home/dev/src/autoguider/commandserver/include/command_server.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/features.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/sys/cdefs.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/gnu/stubs.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/linux/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/asm/errno.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/stdio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/types.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/wordsize.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/typesizes.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/libio.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/_G_config.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/wchar.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/wchar.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/gconv.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/stdio_lim.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/sys_errlist.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/stdlib.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/string.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/unistd.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/posix_opt.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/bits/confname.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /usr/include/getopt.h
/home/dev/bin/autoguider/commandserver/test/i386-linux/test_getfits_command.o: /home/dev/src/autoguider/commandserver/include/command_server.h
