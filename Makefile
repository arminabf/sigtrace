DEBUG ?= 0

CC         = gcc
CFLAGS     = -O2 -Wall -shared -fPIC
LIB_SUFFIX =

ifeq ($(DEBUG), 1)
  CFLAGS += -DDEBUG
  LIB_SUFFIX = -dbg
endif

LIB = sigtrace$(LIB_SUFFIX).so

.phony: clean

all: sigtrace segfault

segfault: segfault.c
	$(CC) -g $< -o $@

sigtrace: sigtrace.c
	$(CC) $(CFLAGS) $< -o $(LIB)


clean:
	rm segfault
	rm -f *.so
