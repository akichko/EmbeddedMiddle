CC            = gcc
CFLAGS        = -g -Wall -fdiagnostics-color=always
LDFLAGS       = -L/usr/local/lib
LIBS          = -lrt -lpthread -lm
SRCDIR        = src
BUILDDIR      = build
OBJDIR        = $(BUILDDIR)/obj
OUTDIR        = $(BUILDDIR)/lib
SRCS          = $(wildcard $(SRCDIR)/*.c)
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
				$(CC) -o $@ -c $<

clean:;         rm -f $(OBJDIR)/*.o *~ $(PROGRAM)
