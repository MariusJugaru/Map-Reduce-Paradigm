#include <locale.h>
#include "mappers.h"
#include "argument.h"

void *mapper(void *arg) {
    ThreadData *thread_data = (ThreadData*)arg;
    InputFiles *input_files = thread_data->input_files;
    int cur_file;
    FILE* fd;
    char *file;

    while (1) {
        // Gets a file for an unread file.
        pthread_mutex_lock(&input_files->mutex);
        if (input_files->cur_file < input_files->num_files) {
            cur_file = input_files->cur_file;
            file = input_files->files[input_files->cur_file++];
        } else {
            pthread_mutex_unlock(&input_files->mutex);
            break;
        }
        pthread_mutex_unlock(&input_files->mutex);

        fd = fopen(file, "r");
        if (!fd) {
            printf("Couldn't open file: %s\n", file);
            return NULL;
        }

        char word[64];
        Hashtable *table = create_table(10);

        while (fscanf(fd, "%63s", word) == 1) {
            char *new_word = ParseWord(word);
            strcpy(word, new_word);
            free(new_word);
            ht_insert(&table, word, cur_file);
        }

        thread_data->tables[cur_file] = table;

        fclose(fd);
    }

    pthread_barrier_wait(&input_files->barrier);
    sem_post(&input_files->sem);
    return NULL;
}

// Removes all unwanted characters and makes all the characters lowercase.
char *ParseWord(char *word) {
    char *ptr = word;
    char *new_word = (char*)malloc(64);
    int idx = 0;

    while (*ptr != '\0') {
        *ptr = tolower(*ptr);

        if (strchr("abcdefghijklmnopqrstuvwxyz", *ptr)) {
            new_word[idx++] = *ptr;
        }
        ptr++;
    }
    new_word[idx] = '\0';

    return new_word;
}