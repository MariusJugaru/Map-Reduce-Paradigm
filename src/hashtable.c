#include "hashtable.h"

// Hash function. Returns an index value.
unsigned long hash_function(Hashtable *table, char *str)
{
    unsigned long i = 0;

    for (int j = 0; str[j]; j++)
        i += str[j];

    return i % table->size;
}

LinkedList* create_cel(int value) {
    LinkedList* cel = (LinkedList*)malloc(sizeof(LinkedList));
    cel->val = value;
    cel->next = NULL;

    return cel;
}

// Creates an item for the hashtable.
Ht_item* create_item(char *key, int value)
{
    Ht_item* item = (Ht_item*)malloc(sizeof(Ht_item));
    if(!item) {
        printf("Memory allocation failed for HT item failed.\n");
        return NULL;
    }

    item->key = (char*)malloc(strlen(key) + 1);
    if (!item->key) {
        printf("Memory allocation failed for HT key failed.\n");
        free(item);
        return NULL;
    }

    item->value = create_cel(value);
    if (!item->value) {
        printf("Memory allocation failed for HT value.\n");
        free(item->key);
        free(item);
        return NULL;
    }

    item->num_values = 1;
    strcpy(item->key, key);
    item->next = NULL;

    return item;
}

// Creates a new HashTable.
Hashtable* create_table(int size)
{
    Hashtable *table = (Hashtable*)malloc(sizeof(Hashtable));
    if (!table) {
        printf("Memory allocation failed for hashtable.\n");
        return NULL;
    }

    table->size = size;
    table->count = 0;

    table->items = (Ht_item**)calloc(table->size, sizeof(Ht_item*));
    if (!table->items) {
        printf("Memory allocation failed for hashtable items.\n");
        free(table);
        return NULL;
    }

    for (int i = 0; i < table->size; i++) {
        table->items[i] = NULL;
    }

    return table;
}

// Frees an item.
void free_item(Ht_item *item)
{
    if (!item)
        return;
    
    free(item->key);
    
    LinkedList *values = item->value;
    while (values) {
        LinkedList *aux = values;
        values = values->next;
        free(aux);
    }

    free(item);
}

// Frees the table.
void free_table(Hashtable *table)
{
    if (!table)
    return;

    for (int i = 0; i < table->size; i++) {
        Ht_item *item = table->items[i];
        while (item) {
            Ht_item *aux = item;
            item = item->next;
            free_item(aux);
        }
    }
    free(table->items);
    free(table);
}

// Prints all the keys: values in the table.
void print_table(Hashtable *table)
{
    if (!table->count)
        return;
    printf("Hash Table\n---------------\n");

    for (int i = 0; i < table->size; i++) {
        Ht_item *item = table->items[i];
        while (item) {
            LinkedList *val_item = item->value;
            printf("Key:%s, Value:", item->key);
            while(val_item) {
                printf("%d", val_item->val);
                val_item = val_item->next;
                if (val_item) {
                    printf(", ");
                }
            }
            printf("\n");
            
            item = item->next;
        }
    }

    printf("---------------\n\n");
}

// Insterts an entry in the table.
void ht_insert(Hashtable **table, char *key, int value)
{
    // hash table is full.
    if ((*table)->count == (*table)->size) {
        ht_resize(table);
    }

    Ht_item *item = create_item(key, value);
    if (!item)
        return;
    int index = hash_function(*table, key);

    Ht_item *current_item = (*table)->items[index];

    // Key does not exist.
    if (current_item == NULL) {
        (*table)->items[index] = item;
        (*table)->count++;
        return;
    }

    // Update the value.
    if (strcmp(current_item->key, key) == 0) {
        current_item->value->val = value;
        free_item(item);
        return;
    }

    // Handle collision
    while(current_item->next != NULL) {
        // Update the value.
        if (strcmp(current_item->key, key) == 0) {
            current_item->value->val = value;
            free_item(item);
            return;
        }

        current_item = current_item->next;
    }
    
    current_item->next = item;
    (*table)->count++;
    
}

// Adds a value to the linked list.
void ht_add_value(Hashtable **table, char *key, int value)
{
    int index = hash_function(*table, key);

    Ht_item *current_item = (*table)->items[index];

    // Key does not exist.
    if (current_item == NULL) {
        ht_insert(table, key, value);
        return;
    }

    // Add the value.
    while (current_item && strcmp(current_item->key, key) != 0) {
        current_item = current_item->next;
    }

    if (!current_item) {
        ht_insert(table, key, value);
        return;
    }

    LinkedList *ptr = current_item->value;

    if (ptr->val == value)
        return;

    LinkedList *val = create_cel(value);
    if (!val) {
        printf("Val allocation failed\n");
        return;
    }

    if (value < ptr->val) {
        val->next = ptr;
        current_item->value = val;
        return;
    }

    while (ptr->next && ptr->next->val < value) {
        ptr = ptr->next;
    }

    if (ptr->next && ptr->next->val == value) {
        free(val);
        return;
    }

    current_item->num_values++;
    val->next = ptr->next;
    ptr->next = val;
}

// Expands the table by twice.
void ht_resize(Hashtable **table)
{
    // Creates a new table.
    Hashtable *new_table = create_table((*table)->size * 2);
    if (!new_table) {
        printf("Hashtable resize failed.\n");
        return;
    }
    new_table->count = (*table)->count;

    // Recomputes the hash function for each key in the old table.
    Ht_item *item, *aux, *aux_new;
    int index;
    
    for (int i = 0; i < (*table)->size; i++) {
        item = (*table)->items[i];
        while (item) {
            aux = item;
            item = item->next;
            aux->next = NULL;

            index = hash_function(new_table, aux->key);
            if (!new_table->items[index])
                new_table->items[index] = aux;
            else {
                aux_new = new_table->items[index];
                while (aux_new->next)
                    aux_new = aux_new->next;
                aux_new->next = aux;
            }
        }
    }

    free((*table)->items);
    free(*table);
    *table = new_table;
}

// Searches for the key in the HashTable.
// Returns -1 if it doesn't exist.
LinkedList* ht_search(Hashtable *table, char *key)
{
    int index = hash_function(table, key);
    Ht_item* item = table->items[index];

    if (!item)
        return NULL;

    if (strcmp(item->key, key) == 0)
        return item->value;

    item = item->next;
    while (item) {
        if (strcmp(item->key, key) == 0)
            return item->value;
    }
    
    return NULL;
}

void ht_delete(Hashtable *table, char *key)
{
    // Deletes an item from the table.
    int index = hash_function(table, key);
    Ht_item *item = table->items[index];

    // Does not exist.
    if (item == NULL)
        return;

    Ht_item *prev = NULL;

    for (; item != NULL; prev = item, item = item->next) {
        if (strcmp(item->key, key) == 0) {
            if (!prev) 
                prev->next = item->next;

            free_item(item);
            table->count--;
            return;
        }
    }
    
}