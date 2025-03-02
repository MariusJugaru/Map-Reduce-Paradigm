#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct linked_list {
    int val;
    struct linked_list *next;
} LinkedList;

typedef struct ht_item {
    char *key;
    LinkedList *value;
    int num_values;
    struct ht_item *next;
} Ht_item;

typedef struct hashtable {
    Ht_item **items;
    int size;
    int count;
} Hashtable;




unsigned long hash_function(Hashtable *table, char *str);
LinkedList* create_cel(int value);
Ht_item* create_item(char *key, int value);
Hashtable* create_table(int size);
void free_item(Ht_item *item);
void free_table(Hashtable *table);
void print_table(Hashtable *table);
void ht_insert(Hashtable **table, char *key, int value);
void ht_add_value(Hashtable **table, char *key, int value);
void ht_resize(Hashtable **table);
LinkedList* ht_search(Hashtable *table, char *key);
void ht_delete(Hashtable *table, char *key);