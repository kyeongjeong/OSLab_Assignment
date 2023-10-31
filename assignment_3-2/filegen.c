#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PROCESSES 10000

int main() {

    char file_name[100];

    if (mkdir("temp", 0777)) {
        perror("fail to make directory");
        return 1;
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {

        snprintf(file_name, sizeof(file_name), "temp/%d", i);

        FILE *file = fopen(file_name, "w");
        fprintf(file, "%d", 1 + rand() % 9); 
        fclose(file);
    }

    return 0;
}
