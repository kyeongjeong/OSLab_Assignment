#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {

    int return_value = syscall(336, 1); 
    printf("return value : %d\n", return_value);
    return 0;
}
