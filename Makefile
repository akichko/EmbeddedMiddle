CC			= gcc
SRCDIR		= ./src
CFLAGS		= -g -Wall -fdiagnostics-color=always -I$(SRCDIR)
LDFLAGS		= -L$(OUTDIR)
LIBS		= -lem -lrt -lpthread -lm -lcurl -lmosquitto
BUILDDIR	= ./build
OBJDIR		= $(BUILDDIR)/obj
OUTDIR		= $(BUILDDIR)/lib
SRCS		= $(wildcard $(SRCDIR)/*.c)
SAMPLESRCS	= $(wildcard sample/*.c)
TESTSRCS	= $(wildcard test/*.c)
APPSRCS		= $(wildcard sampleapp/*.c)
OBJS		= $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
APPOBJS		= $(APPSRCS:app/%.c=$(BUILDDIR)/app/%.o)
AR			= ar
ARFLAGS		= rcs
PROGRAM		= $(OUTDIR)/libem.a
APP			= ./build/app/a.out


all:	$(PROGRAM)

$(PROGRAM):	$(OBJS)
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(AR) $(ARFLAGS) $(PROGRAM) $(OBJS) 

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) -o $@ -c $<


sample:		$(PROGRAM) $(SAMPLESRCS:%.c=$(BUILDDIR)/%)
				
$(BUILDDIR)/sample/%:	sample/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)


test:		$(PROGRAM) $(TESTSRCS:%.c=$(BUILDDIR)/%)
				
$(BUILDDIR)/test/%:	test/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)


app:		$(PROGRAM) $(APP)

$(APP):		$(APPOBJS)
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(APP) $(APPOBJS) $(LIBS)

$(BUILDDIR)/app/%.o:	app/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -f $(OBJDIR)/*.o *~ $(PROGRAM) $(BUILDDIR)/sample/* $(BUILDDIR)/test/* $(BUILDDIR)/app/*
