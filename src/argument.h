#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "hashtable.h"

typedef struct files {
    int num_files;
    char **files;
    int cur_file;
    pthread_mutex_t mutex;
    pthread_barrier_t barrier;
    sem_t sem;
} InputFiles;

typedef struct thread_data {
    int id;
    int *cur_out_file;
    Hashtable **tables;
    InputFiles *input_files;
} ThreadData;

// Create argument for the mappers. Contains file descriptors to all of the files.
InputFiles* CreateInputFiles(FILE* input_file, int num_of_mappers);
void FreeInputFiles(InputFiles *input_files);