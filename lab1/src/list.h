#ifndef LABMOD_LIST_H 
#define LABMOD_LIST_H

#include <stddef.h>

struct list_impl
{
    size_t value;
    struct list_impl* next;
};

struct list  
{
    int size;
    struct list_impl* head;
    struct list_impl* tail;
};

int add_to_list(struct list* target, size_t value);
int get_list_size(struct list* target);
void destroy_list(struct list* target);
 
#endif // LABMOD_LIST_H