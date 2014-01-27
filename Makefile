all:      lpc21isp

GLOBAL_DEP  = adprog.h lpc21isp.h lpcprog.h lpcterm.h
CC = gcc

ifneq ($(findstring(freebsd, $(OSTYPE))),)
CFLAGS+=-D__FREEBSD__
endif

ifeq ($(OSTYPE),)
OSTYPE		= $(shell uname)
endif

ifneq ($(findstring Darwin,$(OSTYPE)),)
CFLAGS+=-D__APPLE__
endif

CFLAGS	+= -Wall -Wextra -O2 -flto -s

objects=lpcprog.o adprog.o lpcterm.o lpc21isp.o errors.o

lpc21isp: $(objects) $(GLOBAL_DEP)
	$(CC) -o lpc21isp $(CDEBUG) $(CFLAGS) $(objects)
%.o: %.c
	$(CC) $(CDEBUG) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(objects) lpc21isp
