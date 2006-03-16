# $Header: /home/cjm/cvs/commandserver/c/Makefile,v 1.2 2006-03-16 11:11:35 cjm Exp $

include ../../../Makefile.common
include ../../Makefile.common
include ../Makefile.common

BINDIR			= $(AUTOGUIDER_COMMANDSERVER_BIN_HOME)/$(HOSTTYPE)
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
LIB_OBJS	= $(LIB_SRCS:%.c=%.o)
OBJS		= $(SRCS:%.c=%.o)
DOCS 		= $(SRCS:%.c=$(DOCSDIR)/%.html)

top: $(LT_LIB_HOME)/$(LIBRARYNAME).so docs

static: $(LT_LIB_HOME)/$(LIBRARYNAME).so

$(LT_LIB_HOME)/$(LIBRARYNAME).so : $(LIB_OBJS)
	$(CC) $(CCSHAREDFLAG) $(CFLAGS) $(LIB_OBJS) -o $@

$(LT_LIB_HOME)/$(LIBRARYNAME).a : $(LIB_OBJS)
	ar rcv $@ $?

$(BINDIR)/%: %.o
	$(CC) $< -o $@  -L$(LT_LIB_HOME) $(TIMELIB) $(SOCKETLIB) -lm -lc

docs: $(DOCS)

$(DOCS): $(SRCS)
	-$(CDOC) -d $(DOCSDIR) -h $(INCDIR) $(DOCFLAGS) $(SRCS)

$(DOCS) : $(SRCS)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

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

