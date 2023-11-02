#include <stdio.h>
#define MAX_PROCESSES 64

int main() {

    FILE *file = fopen("temp.txt", "w");

    if (file == NULL) {
        perror("Fail to open file");
        return 1;
    }

    for (int i = 0; i < (MAX_PROCESSES * 2); i++) {

        fprintf(file, "%d\n", i + 1);
    }
    fclose(file);
    return 0;
}
