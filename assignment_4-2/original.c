#include <stdio.h>
#include <stdint.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>

int Operation(int a)
{
   __asm__(
        ".intel_syntax;"
        "mov %%eax, %1;"
        "mov %%dl, 2;"
        "add %%eax, 1;"
        "add %%eax, 2;"
        "add %%eax, 3;"
        "add %%eax, 4;"
        "add %%eax, 5;"
        "add %%eax, 6;"
        "add %%eax, 7;"
        "add %%eax, 8;"
        "add %%eax, 9;" // add 45
   "imul %%eax, 2;" 
   "imul %%eax, 2;"
   "imul %%eax, 2;" // mul 8
        "add %%eax, 1;" 
   "add %%eax, 1;"
        "add %%eax, 3;"
        "add %%eax, 1;"
        "add %%eax, 1;"
        "add %%eax, 1;"
        "add %%eax, 3;"
   "add %%eax, 1;"
   "add %%eax, 1;"
        "add %%eax, 2;"
        "add %%eax, 1;"
        "add %%eax, 1;"
   "add %%eax, 1;"
        "add %%eax, 1;"
        "add %%eax, 1;" // add 20 
        "div %%dl;"
        "div %%dl;" // div 4
        "sub %%eax, 1;" 
   "sub %%eax, 1;"
        "sub %%eax, 3;"
        "sub %%eax, 1;"
        "sub %%eax, 1;"
        "sub %%eax, 1;"
        "sub %%eax, 3;"
   "sub %%eax, 1;"
   "sub %%eax, 1;"
        "sub %%eax, 2;"
        "sub %%eax, 1;"
        "sub %%eax, 1;"
   "sub %%eax, 1;"
        "sub %%eax, 1;"
        "sub %%eax, 1;" // sub 20
        "mov %0, %%eax;"
        ".att_syntax;": "=r"(a): "r"(a) :"eax"
   );
   return a;
}

int main(void)
{
   uint8_t* func = (uint8_t*)Operation;
   int i = 0;
   int segment_id;
   uint8_t* shared_memory;

   segment_id = shmget(1234, PAGE_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
   shared_memory = (uint8_t*)shmat(segment_id, NULL, 0);

   do {
           shared_memory[i++] = *func;
   } while (*func++ != 0xC3);

   printf("result: %d \n", Operation(1));
   shmdt(shared_memory);
   printf("Data was filled to shared memory.\n");
        
   return 0;
}
