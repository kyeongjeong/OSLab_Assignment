#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>          
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>  
#include <string.h>         
#define MAX_PROCESSES 10000

int main() {

    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    for (int i = 0; i < MAX_PROCESSES; i++) {

        pid_t pid = fork();
        struct sched_param params;
        params.sched_priority = 50;

        if (sched_setscheduler(0, SCHED_RR, &params) == -1) {
            perror("sched_setscheduler error");
            exit(1);
        }
        if (setpriority(PRIO_PROCESS, 0, 0) == -1) {
            perror("setpriority error\n");
            exit(1);
        }
        
        if (pid == -1) {
            perror("Fork error");
            exit(1);
        } 
        else if (pid == 0) {
            
            char filename[100];
            snprintf(filename, sizeof(filename), "temp/%d", i);

            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                perror("Fail to open file\n");
                exit(1);
            }

            int data;
            if (fscanf(file, "%d", &data) != 1) {
                perror("Fail to read file\n");
                exit(1);
            }
            fclose(file);
            exit(0);
        }
    }

    int status;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        wait(&status);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Execution time : %f\n", execution_time);

    return 0;
}
