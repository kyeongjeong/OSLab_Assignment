all: numgen fork thread

numgen: numgen.c
   gcc -o numgen numgen.c

fork: fork.c
   gcc -o fork fork.c

thread: thread.c
   gcc -pthread -o thread thread.c

clean:
   rm -f numgen fork thread
