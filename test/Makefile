# $Header: /home/cjm/cvs/commandserver/test/Makefile,v 1.3 2006-05-26 10:07:04 cjm Exp $

include ../../../Makefile.common
include ../../Makefile.common
include ../Makefile.common

TEST_HOME		= test
BINDIR			= $(AUTOGUIDER_COMMANDSERVER_BIN_HOME)/$(TEST_HOME)/$(HOSTTYPE)
INCDIR 			= $(AUTOGUIDER_COMMANDSERVER_SRC_HOME)/include
DOCSDIR 		= $(AUTOGUIDER_COMMANDSERVER_DOC_HOME)/$(TEST_HOME)

#DEBUG_CFLAGS	= 
DEBUG_CFLAGS	= -DCOMMAND_SERVER_DEBUG 
#-DGETFITS_DEBUG
#-DGETFITS_DEBUG2

CFLAGS 		= -g -I$(INCDIR) -I$(CFITSIOINCDIR) $(DEBUG_CFLAGS)
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

test_server.o: /usr/include/errno.h /usr/include/features.h
test_server.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
test_server.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
test_server.o: /usr/include/asm/errno.h /usr/include/malloc.h
test_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
test_server.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
test_server.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
test_server.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
test_server.o: /usr/include/getopt.h /usr/include/stdio.h
test_server.o: /usr/include/libio.h /usr/include/_G_config.h
test_server.o: /usr/include/wchar.h /usr/include/bits/wchar.h
test_server.o: /usr/include/gconv.h
test_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
test_server.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
test_server.o: /usr/include/stdlib.h /usr/include/string.h
test_server.o: /home/dev/src/cfitsio3.006/include/fitsio.h
test_server.o: /usr/include/limits.h
test_server.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
test_server.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
test_server.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
test_server.o: /home/dev/src/cfitsio3.006/include/longnam.h
test_server.o: /home/dev/src/autoguider/commandserver/include/command_server.h
send_command.o: /usr/include/stdio.h /usr/include/features.h
send_command.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
send_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
send_command.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
send_command.o: /usr/include/bits/typesizes.h /usr/include/libio.h
send_command.o: /usr/include/_G_config.h /usr/include/wchar.h
send_command.o: /usr/include/bits/wchar.h /usr/include/gconv.h
send_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
send_command.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
send_command.o: /usr/include/stdlib.h /usr/include/string.h
send_command.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
send_command.o: /usr/include/bits/confname.h /usr/include/getopt.h
send_command.o: /home/dev/src/autoguider/commandserver/include/command_server.h
test_getfits_command.o: /usr/include/errno.h /usr/include/features.h
test_getfits_command.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
test_getfits_command.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
test_getfits_command.o: /usr/include/asm/errno.h /usr/include/stdio.h
test_getfits_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
test_getfits_command.o: /usr/include/bits/types.h
test_getfits_command.o: /usr/include/bits/wordsize.h
test_getfits_command.o: /usr/include/bits/typesizes.h /usr/include/libio.h
test_getfits_command.o: /usr/include/_G_config.h /usr/include/wchar.h
test_getfits_command.o: /usr/include/bits/wchar.h /usr/include/gconv.h
test_getfits_command.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
test_getfits_command.o: /usr/include/bits/stdio_lim.h
test_getfits_command.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
test_getfits_command.o: /usr/include/string.h /usr/include/unistd.h
test_getfits_command.o: /usr/include/bits/posix_opt.h
test_getfits_command.o: /usr/include/bits/confname.h /usr/include/getopt.h
test_getfits_command.o: /home/dev/src/autoguider/commandserver/include/command_server.h
