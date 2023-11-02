#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#define MAX_PROCESSES 64

int num_array[MAX_PROCESSES * 2];
int result = 0;
pthread_mutex_t mutex;

void* sum_thread(void* args);

int main() {

    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    FILE *file = fopen("temp.txt", "r");
    if(file == NULL) {
        perror("Fail to open file");
        exit(1);
    }

    for (int i = 0; i < MAX_PROCESSES * 2; i++) 
        fscanf(file, "%d", &num_array[i]);

    pthread_mutex_init(&mutex, NULL);

    int thread_args[] = { 0, MAX_PROCESSES * 2 - 1 };
    pthread_t root_thread;
    pthread_create(&root_thread, NULL, sum_thread, thread_args);
    pthread_join(root_thread, NULL);

    pthread_mutex_destroy(&mutex);

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Value of fork: %d\n", result);
    printf("%f\n", execution_time);

    fclose(file);
    return 0;
}

void* sum_thread(void* args) {

    int* params = (int*)args;
    int start = params[0];
    int end = params[1];

    if (start == end) {

        pthread_mutex_lock(&mutex);
        result += num_array[start];
        pthread_mutex_unlock(&mutex);
    } 
    else {

        int middle = (start + end) / 2;
        
        int thread_args1[] = { start, middle };
        int thread_args2[] = { middle + 1, end };

        pthread_t thread1, thread2;
        pthread_create(&thread1, NULL, sum_thread, thread_args1);
        pthread_create(&thread2, NULL, sum_thread, thread_args2);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }
    return NULL;
}
