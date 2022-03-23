#include "list.h"

#include <linux/slab.h>

int add_to_list(struct list* target, size_t value)
{
    struct list_impl* node;

    if (target == NULL) return -1;

    node = kmalloc(sizeof(struct list_impl), GFP_KERNEL);

    if (node == NULL) return -1;

    node->value = value;
    node->next = NULL;

    if (target->size == 0)
    {
        target->size = 1;
        target->head = node;
        target->tail = node;
    }
    else
    {
        target->size++;
        target->tail->next = node;
        target->tail = node;
    }

    return 0;
}

int get_list_size(struct list* target)
{
    if (target == NULL) return -1;

    return target->size;
}

void destroy_list(struct list* target)
{
    struct list_impl *last, *it;

    if (target == NULL) return;

    for (last = NULL, it = target->head; it != NULL; last = it, it = it->next)
    {
       kfree(last);
    }

    kfree(last);
}
