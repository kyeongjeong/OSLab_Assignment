#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <time.h>
#include <sched.h> 

uint8_t* Operation;
uint8_t* compiled_code;
int segment_id;

void sharedmem_init(); // access to shared memory
void sharedmem_exit();
void drecompile_init(); // memory mapping start
void drecompile_exit(); 
void* drecompile(uint8_t *func); // optimization

int main(void)
{
    int (*func)(int a);
    struct timespec start_time, end_time;

    sharedmem_init();
    drecompile_init(Operation);
    func = (int (*)(int a))drecompile(Operation);

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    func(1);
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("total execution time: %f sec\n", execution_time);

    drecompile_exit();
    sharedmem_exit();
    return 0;
}

void sharedmem_init() 
{
    segment_id = shmget(1234, PAGE_SIZE, IPC_CREAT | 0666);
    Operation = (uint8_t*)shmat(segment_id, NULL, 0);
}

void sharedmem_exit()
{
    shmdt(Operation);
    shmctl(segment_id, IPC_RMID, NULL);
}

void drecompile_init(uint8_t *func)
{
    compiled_code = (uint8_t *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);
}

void drecompile_exit()
{
    munmap(compiled_code, PAGE_SIZE);
}

void* drecompile(uint8_t* func)
{
#ifdef dynamic
    int i = 0, j = 0, k = 0, isFirst = 1, dl;
    uint8_t operand = 0, nextOperand = 0;
    do {
        if(func[i] == 0xb2 && isFirst == 1) {
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            dl = func[i + 1];
            i += 2;
            k += 2;
            isFirst == 0;
        }

        else if (func[i] == 0x83 && func[i + 1] == 0xc0) {
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            operand = func[i + 2];

            for (j = i + 3; func[j] != 0xC3;) {
                if (func[j] == 0x83 && func[j + 1] == 0xc0) {

                    nextOperand = func[j + 2];
                    operand += nextOperand;
                    j += 3;
                } 
            else 
                break;
            }
            i = j;
            compiled_code[k + 2] = operand;
            k += 3;
        }

        else if (func[i] == 0x83 && func[i + 1] == 0xe8) {        
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            operand = func[i + 2];
            
            for (j = i + 3; func[j] != 0xC3;) {
                if (func[j] == 0x83 && func[j + 1] == 0xe8) {

                    nextOperand = func[j + 2];
                    operand += nextOperand;
                    j += 3;
                } 
                else 
                    break;
            }
            i = j;
            compiled_code[k + 2] = operand;
            k += 3;
        }

        else if (func[i] == 0x6b) {   
            compiled_code[k] = func[i];
            compiled_code[k + 1] = func[i + 1];
            operand = func[i + 2];
            
            for (j = i + 3; func[j] != 0xc3;) {
                if (func[j] == 0x6b) {

                    nextOperand = func[j + 2];
                    operand *= nextOperand;
                    j += 3;
                } 
                else 
                    break;
            }
            i = j;
            compiled_code[k + 2] = operand;
            k += 3;
        }

        else if (func[i] == 0xf6) {    
            
            operand = dl;
            for (j = i + 2; func[j] != 0xc3;) {
                if (func[j] == 0xf6) {

                    operand *= dl;
                    j += 2;
                } 
                else 
                    break;
            }
            i = j;    
            compiled_code[k] = 0xb2;
            compiled_code[k + 1] = operand;
            compiled_code[k + 2] = 0xf6;
            compiled_code[k + 3] = 0xf2;
            k += 4;
        }

        else {
            compiled_code[k] = func[i];
            ++i;
            ++k;
        }
    } while(func[i] != 0xC3);
    compiled_code[k++] = 0xc3;

#else
    int i = 0;
    do {
        compiled_code[i++] = *func;
    } while (*func++ != 0xC3);
    compiled_code[i] = 0xC3;
    
#endif

    mprotect(compiled_code, PAGE_SIZE, PROT_READ | PROT_EXEC);
    return compiled_code;
}
