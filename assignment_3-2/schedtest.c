#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#define MAX_PROCESSES 10000

int main() {

    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    for (int i = 0; i < MAX_PROCESSES; i++) {

        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork error");
            exit(1);
        } 
        else if (pid == 0) {
            
            char filename[100];
            snprintf(filename, sizeof(filename), "temp/%d", i);

            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                printf("Fail to open file\n");
                exit(1);
            }

            int data;
            if (fscanf(file, "%d", &data) != 1) {
                printf("Fail to read file\n");
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
