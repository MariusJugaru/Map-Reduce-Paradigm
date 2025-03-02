#include "reducers.h"
#include "argument.h"
#define ALPHABET_COUNT 26


int compare(const void *a, const void *b) {
    Ht_item* item_a = *(Ht_item**)a;
    Ht_item* item_b = *(Ht_item**)b;

    if (item_a->num_values > item_b->num_values)
        return -1;
    if (item_a->num_values < item_b->num_values)
        return 1;
    
    return strcmp(item_a->key, item_b->key);
}

void *reducer(void *arg) {
    ThreadData *thread_data = (ThreadData*)arg;
    InputFiles *input_files = thread_data->input_files;

    // Wait for the mappers to finish
    sem_wait(&input_files->sem);
    sem_post(&input_files->sem);

    int cur_out_file;
    char out_file[6];
    FILE* out_fd;

    while (1) {
        // Gets a file for an out file.
        pthread_mutex_lock(&input_files->mutex);
        if (*(thread_data->cur_out_file) < ALPHABET_COUNT) {
            cur_out_file = *(thread_data->cur_out_file);
            (*(thread_data->cur_out_file))++;
        } else {
            pthread_mutex_unlock(&input_files->mutex);
            break;
        }
        pthread_mutex_unlock(&input_files->mutex);

        char partition = 'a' + cur_out_file;
        *out_file = partition;
        strcpy(out_file + 1, ".txt");

        out_fd = fopen(out_file, "w");
        if (!out_fd) {
            printf("File couldn't be created.\n");
            break;
        }

        // Creates a new hash table with only the words that start with 'a' + cur_out_file and combine the documents number.
        Hashtable *aggregated_table = create_table(5);

        for (int i = 0; i < input_files->num_files; i++) {
            Hashtable *table = thread_data[0].tables[i];

            for (int i = 0; i < table->size; i++) {
                Ht_item *item = table->items[i];
    
                while (item) {
                    if ((*(item->key)) == partition) {
                        LinkedList *val_item = item->value;
                        
                        while(val_item) {
                            ht_add_value(&aggregated_table, item->key, val_item->val);
                            val_item = val_item->next;
                        }
                    }
                    item = item->next;
                }
            }
        }

        if (!aggregated_table->count) {
            free_table(aggregated_table);
            fclose(out_fd);
            continue;
        }

        // Creates an item list for sorting from the aggregated hash table.
        Ht_item **items = (Ht_item**)calloc(aggregated_table->count, sizeof(Ht_item*));
        int idx = 0;

        for (int i = 0; i < aggregated_table->size; i++) {
            Ht_item *item = aggregated_table->items[i];

            while (item) {
                items[idx] = item;

                item = item->next;
                idx++;
            }
        }

        qsort(items, aggregated_table->count, sizeof(Ht_item*), compare);

        for (int i = 0; i < aggregated_table->count; i++) {
            fprintf(out_fd, "%s:[", items[i]->key);
            LinkedList *ptr = items[i]->value;
            while (ptr) {
                fprintf(out_fd, "%d", ptr->val + 1);
                if (ptr->next)
                    fprintf(out_fd, " ");
                ptr = ptr->next;
            }
            fprintf(out_fd, "]\n");
        }
        
        free(items);
        free_table(aggregated_table);
        fclose(out_fd);
    }
    return NULL;
}