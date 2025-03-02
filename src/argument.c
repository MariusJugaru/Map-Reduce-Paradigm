#include "argument.h"

InputFiles* CreateInputFiles(FILE* input_file, int num_of_mappers) {
    
    InputFiles *input_files = (InputFiles*)malloc(sizeof(InputFiles));
    if (!input_files) {
        printf("Memory allocation failed for input files.\n");
        return 0;
    }

    char buff[256];
    fgets(buff, sizeof(buff), input_file);
    input_files->num_files = atoi(buff);

    input_files->files = (char**)calloc(input_files->num_files, sizeof(char*));
    if (!input_files->files) {
        printf("Memory allocation failed for files.\n");
        free(input_files);
        return 0;
    }
    input_files->cur_file = 0;

    // Create files
    for (int i = 0; i < input_files->num_files; i++) {
        fgets(buff, sizeof(buff), input_file);
        buff[strcspn(buff, "\n")] = '\0';

        input_files->files[i] = (char*)malloc(strlen(buff) + 1);
        if(!input_files->files[i]) {
            printf("Couldn't alloc file name.\n");
            free(input_files->files);
            free(input_files);
            return 0;
        }

        strcpy(input_files->files[i], buff);
    }

    pthread_mutex_init(&input_files->mutex, NULL);
    pthread_barrier_init(&input_files->barrier, NULL, num_of_mappers);
    sem_init(&input_files->sem, 0, 0);

    return input_files;
}

void FreeInputFiles(InputFiles *input_files) {
    for (int i = 0; i < input_files->num_files; i++) {
        free(input_files->files[i]);
    }
    free(input_files->files);
    free(input_files);
}