CC = gcc
CFLAGS = -pthread

all: numgen fork thread

numgen: numgen.c
   $(CC) $(CFLAGS) -o numgen numgen.c

fork: fork.c
   $(CC) $(CFLAGS) -o fork fork.c

thread: thread.c
   $(CC) $(CFLAGS) -o thread thread.c

clean:
   rm -f numgen fork thread
   