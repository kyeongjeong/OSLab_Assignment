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
   //printf("sharedmem_init start\n");

   segment_id = shmget(1234, PAGE_SIZE, IPC_CREAT | 0666);
    Operation = (uint8_t*)shmat(segment_id, NULL, 0);

   //printf("sharedmem_init end\n");
}

void sharedmem_exit()
{
   //printf("sharedmem_exit start\n");

   shmdt(Operation);
   shmctl(segment_id, IPC_RMID, NULL);

   //printf("sharedmem_exit endt\n");
}

void drecompile_init(uint8_t *func)
{
   //printf("drecompile_init start\n");
   //int i = 0;
   compiled_code = (uint8_t *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);

   // do {
    //     compiled_code[i] = *func;
   //    printf("%x\n", compiled_code[i]);
   //    i++;
   //    // compiled_code[i++] = *func;
    // } while (*func++ != 0xC3);

   //printf("drecompile_init end\n");
}

void drecompile_exit()
{
   //printf("drecompile_exit start\n");

   munmap(compiled_code, PAGE_SIZE);
   
    //printf("drecompile_exit end\n");
}

void* drecompile(uint8_t* func)
{
   //printf("drecompile start\n");
   
   int i = 0, j = 0, k = 0;
   uint8_t operand = 0, nextOperand = 0, prev = 0;

   do {
      if (func[i] == 0x83 && func[i + 1] == 0xc0) {
            
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            operand = func[i + 2];
            
            for (j = i + 3; func[j] != 0xC3;) {
                if (func[j] == 0x83 && func[j + 1] == 0xc0) {

                    prev = operand;
                    nextOperand = func[j + 2];
                    operand += nextOperand;
                    j += 3;
                    printf("prev:%x + next:%x = %x\n", prev, nextOperand, operand);
                } 
            else 
                    break;
            }
            i = j;
            compiled_code[k + 2] = operand;
            printf("%d:%x %d:%x %d:%x\n", k, compiled_code[k], k + 1, compiled_code[k + 1], k + 2, compiled_code[k + 2]);
         k += 3;
        }

      else if (func[i] == 0x83 && func[i + 1] == 0xe8) {
            
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            operand = func[i + 2];
            
            for (j = i + 3; func[j] != 0xC3;) {
                if (func[j] == 0x83 && func[j + 1] == 0xe8) {

                    prev = operand;
                    nextOperand = func[j + 2];
                    operand += nextOperand;
                    j += 3;
                    printf("prev:%x + next:%x = %x\n", prev, nextOperand, operand);
                } 
            else 
                    break;
            }
            i = j;
            compiled_code[k + 2] = operand;
            printf("%d:%x %d:%x %d:%x\n", k, compiled_code[k], k + 1, compiled_code[k + 1], k + 2, compiled_code[k + 2]);
         k += 3;
        }

      else if (func[i] == 0x6b) {
            
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            operand = func[i + 2];
            
            for (j = i + 3; func[j] != 0xc3;) {
                if (func[j] == 0x6b) {

                    prev = operand;
                    nextOperand = func[j + 2];
                    operand *= nextOperand;
                    j += 3;
                    printf("prev:%x * next:%x = %x\n", prev, nextOperand, operand);
                } 
            else 
                    break;
            }
            i = j;
            compiled_code[k + 2] = operand;
            printf("%d:%x %d:%x %d:%x\n", k, compiled_code[k], k + 1, compiled_code[k + 1], k + 2, compiled_code[k + 2]);
         k += 3;
        }

      else if (func[i] == 0xf6) {
            
            operand = 2;

            for (j = i + 3; func[j] != 0xc3;) {
                if (func[j] == 0xf6) {

                    prev = operand;
                    // nextOperand = func[j + 2];
                    operand *= 2;
                    j += 3;
                    printf("prev:%x + next:%x = %x\n", prev, 2, operand);
                } 
            else 
                    break;
            }
            i = j; 
            
            compiled_code[k] = 0xb2;
            compiled_code[k + 1] = operand;

            compiled_code[k + 2] = 0xf6;
            compiled_code[k + 3] = 0xf2;

            printf("%d:%x %d:%x\n", k, compiled_code[k], k + 1, compiled_code[k + 1]);
         k += 4;
        }

        else {
            ++i;
        }
    } while(func[i] != 0xC3);
    
    for(i = 0; i < k;) {

    }

   // for(i = 0; i < k; i += 3)
   //    printf("%d:%x %d:%x %d:%x\n", i, compiled_code[i], i + 1, compiled_code[i + 1], i + 2, compiled_code[i + 2]);

   //printf("drecompile end\n");
}
