EXEC = D_recompile
CC = gcc

default:
   $(CC) -o run D_recompile.c

dynamic:
   $(CC) -Ddynamic -o run D_recompile.c

clean:
   rm -rf D_recompile $(EXEC)
   sync
   echo 3 | sudo tee /proc/sys/vm/drop_caches
