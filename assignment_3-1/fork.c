#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#define MAX_PROCESSES 8

int recursive_sum(int num_array[], int start, int end);

int main() {
    
    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    FILE *file = fopen("temp.txt", "r");
    if(file == NULL) {
        perror("Fail to open file");
        exit(1);
    }

    int num_array[MAX_PROCESSES * 2];
    for (int i = 0; i < MAX_PROCESSES * 2; i++) 
        fscanf(file, "%d", &num_array[i]);

    int result = recursive_sum(num_array, 0, MAX_PROCESSES * 2 - 1);

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Value of fork : %d\n", result); 
    printf("%f\n", execution_time);

    fclose(file);
    return 0;
}

int recursive_sum(int num_array[], int start, int end) {

    pid_t pid1, pid2;
    int status1, status2;
    int child_result1, child_result2;
    
    if (start == end) 
        return num_array[start];

    int middle = (start + end) / 2;

    pid1 = fork();
    if (pid1 == -1) {        
        perror("Fork error\n");
        exit(1);
    } 
    else if (pid1 == 0) {
        child_result1 = recursive_sum(num_array, start, middle);
        exit(child_result1);
    }

    pid2 = fork();
    if (pid2 == -1) {
        perror("Fork error\n");
        exit(1);
    } 
    else if (pid2 == 0) {
        child_result2 = recursive_sum(num_array, middle + 1, end);
        exit(child_result2);
    }

    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    child_result1 = status1 >> 8;
    child_result2 = status2 >> 8;

    return child_result1 + child_result2;
}
