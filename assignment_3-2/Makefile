CC = gcc

all: filegen schedtest

filegen: filegen.c
   $(CC) $(CFLAGS) -o filegen filegen.c

schedtest: schedtest.c
   $(CC) $(CFLAGS) -o schedtest schedtest.c

clean:
   rm -f filegen schedtest
