CC            = gcc
SRCDIR        = ./src
CFLAGS        = -g -Wall -fdiagnostics-color=always -I$(SRCDIR)
LDFLAGS       = -L$(OUTDIR)
LIBS          = -lem -lrt -lpthread -lm -lcurl -lmosquitto
BUILDDIR      = ./build
OBJDIR        = $(BUILDDIR)/obj
OUTDIR        = $(BUILDDIR)/lib
SRCS          = $(wildcard $(SRCDIR)/*.c)
SAMPLESRCS    = $(wildcard sample/*.c)
OBJS          = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
AR            = ar
ARFLAGS       = rcs
PROGRAM       = $(OUTDIR)/libem.a

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
				@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
				$(AR) $(ARFLAGS) $(PROGRAM) $(OBJS) 

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
				@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
				$(CC) $(CFLAGS) -o $@ -c $<

sample:			$(PROGRAM) $(SAMPLESRCS:%.c=$(BUILDDIR)/%)
				
$(BUILDDIR)/sample/%:	sample/%.c
						@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
						$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

clean:;         rm -f $(OBJDIR)/*.o *~ $(PROGRAM) $(BUILDDIR)/sample/*
