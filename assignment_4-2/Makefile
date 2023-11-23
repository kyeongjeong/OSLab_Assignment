EXEC = D_recompile
CC = gcc

default:
   $(CC) -o drecompile D_recompile.c

dynamic:
   $(CC) -Ddynamic -o drecompile D_recompile.c

clean:
   rm -rf D_recompile $(EXEC)
   sync
   echo 3 | sudo tee /proc/sys/vm/drop_caches
