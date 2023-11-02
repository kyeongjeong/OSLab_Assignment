#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PROCESSES 10000

int main() {

    char file_name[100];

    if (mkdir("temp", 0777)) {
        perror("Fail to make directory");
        return 1;
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {

        snprintf(file_name, sizeof(file_name), "temp/%d", i);

        FILE *file = fopen(file_name, "w");
        int rand_num = 1 + rand() % 9;
        fprintf(file, "%d", rand_num);
        fclose(file);

        if(i % 1000 == 0)
            printf("value of temp/%d : %d\n", i, rand_num);
    }

    return 0;
}
