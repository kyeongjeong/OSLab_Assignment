#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/mman.h>

uint8_t* Operation;
uint8_t* compiled_code;
int segment_id;

void sharedmem_init(); // 공유 메모리에 접근
void sharedmem_exit();
void drecompile_init(); // memory mapping 시작
void drecompile_exit(); 
void* drecompile(uint8_t *func); // 최적화하는 부분

int main(void)
{
   int (*func)(int a);

   sharedmem_init();
   drecompile_init(Operation);

   func = (int (*)(int a))drecompile(Operation);

   drecompile_exit();
   sharedmem_exit();
   return 0;
}

void sharedmem_init() 
{
   segment_id = shmget(1234, PAGE_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
   Operation = (uint8_t*)shmat(segment_id, NULL, 0);
}

void sharedmem_exit()
{
   shmdt(compiled_code);
   shmctl(segment_id, IPC_RMID, NULL);
}

void drecompile_init(uint8_t *func)
{
   compiled_code = (uint8_t*)mmap(NULL, PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, -1, 0);

   do {
       compiled_code[i++] = *func;
   } while (*func++ != 0xC3);
}

void drecompile_exit()
{
   munmap(compiled_code, PAGE_SIZE);
}

void* drecompile(uint8_t* func)
{
}
