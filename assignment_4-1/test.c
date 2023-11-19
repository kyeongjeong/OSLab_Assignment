#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>

#define __NR_ftrace 336

int main(){
    
    int return_value = syscall(__NR_ftrace, getpid());
    printf("return value : %d\n", return_value);
    return 0;
}
