#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "argument.h"
#include "mappers.h"
#include "reducers.h"

int main(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: ./tema1 <number_of_mappers> <number_of_reducers> <input_file>\n");
        return 0;
    }

    // Get arguments
    int number_of_mappers;
    int number_of_reducers;
    FILE *input_file;

    number_of_mappers = atoi(argv[1]);
    number_of_reducers = atoi(argv[2]);
    input_file = fopen(argv[3], "r");

    if(!input_file) {
        printf("The file couldn't be opened.\n");
        return 0;
    }
    
    InputFiles *input_files = CreateInputFiles(input_file, number_of_mappers);

    // Create threads
    int num_threads = number_of_mappers + number_of_reducers;
    pthread_t threads[num_threads];
    int r;
    long id;
    void *status;
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    Hashtable** tables = (Hashtable**)calloc(input_files->num_files, sizeof(Hashtable*));
    int *cur_out_file = (int*)malloc(sizeof(int));
    *cur_out_file = 0;
 
    for (id = 0; id < num_threads; id++) {
        thread_data[id].cur_out_file = cur_out_file;
        thread_data[id].tables = tables;
        thread_data[id].input_files = input_files;
        if (id < number_of_mappers) {
            thread_data[id].id = id;
            r = pthread_create(&threads[id], NULL, mapper,  (void*)&thread_data[id]);
        } else {
            thread_data[id].id = id - number_of_mappers;
            r = pthread_create(&threads[id], NULL, reducer,  (void*)&thread_data[id]);
        }
            
        if (r) {
            printf("Error creating thread %ld\n", id);
            exit(-1);
        }
    }
 
    for (id = 0; id < num_threads; id++) {
        r = pthread_join(threads[id], &status);
 
        if (r) {
            printf("Error waiting thread %ld\n", id);
            exit(-1);
        }
    }

    // Free memory
    pthread_mutex_destroy(&input_files->mutex);
    pthread_barrier_destroy(&input_files->barrier);
    sem_destroy(&input_files->sem);

    for (int i = 0; i < input_files->num_files; i++) {
        free_table(tables[i]);
    }
    free(tables);
    free(cur_out_file);
    FreeInputFiles(input_files);
    free(thread_data);
    fclose(input_file);
    return 0;
}