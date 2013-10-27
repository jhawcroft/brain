//
//  array.c
//  AI-NLPartA-IdeaA
//
//  Created by Joshua Hawcroft on 12/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "array.h"



struct Array_
{
    void **items;
    size_t count;
    size_t iterator_index;
};



Array* array_create(void)
{
    Array *array = calloc(1, sizeof(Array));
    return array;
}


Array* array_clone(Array *in_array)
{
    Array *array = array_create();
    if (in_array->count > 0)
        array->items = malloc(sizeof(void*) * in_array->count);
    array->count = in_array->count;
    memcpy(array->items, in_array->items, sizeof(void*) * (in_array->count));
    return array;
}


void array_dispose(Array *in_array)
{
    if (in_array->items) free(in_array->items);
    free(in_array);
}


void array_insert(Array *in_array, size_t in_at_index, void *in_item)
{
    void **new_items = realloc(in_array->items, sizeof(void*) * (in_array->count + 1));
    if (!new_items)
    {
        fprintf(stderr, "array_insert(): out of memory\n");
        abort();
    }
    in_array->items = new_items;
    memmove(&(new_items[in_at_index + 1]), &(new_items[in_at_index]), sizeof(void*) * (in_array->count - in_at_index));
    new_items[in_at_index] = in_item;
    in_array->count++;
}


void array_append(Array *in_array, void *in_item)
{
    array_insert(in_array, in_array->count, in_item);
}


void array_remove(Array *in_array, size_t in_index)
{
    memmove(&(in_array->items[in_index]), &(in_array->items[in_index + 1]), sizeof(void*) * (in_array->count - in_index - 1));
    in_array->count--;
}


size_t array_count(Array *in_array)
{
    return in_array->count;
}


void* array_item(Array *in_array, size_t in_index)
{
    return in_array->items[in_index];
}


void array_intersect(Array *in_dest, Array *in_src)
{
    for (size_t d = 0; d < in_dest->count; d++)
    {
        void *dest_item = in_dest->items[d];
        for (size_t s = 0; s < in_src->count; s++)
        {
            if (in_src->items[s] == dest_item) goto intersection_ds_found_item;
        }
        array_remove(in_dest, d);
    intersection_ds_found_item:
        continue;
    }
}


void array_union(Array *in_dest, Array *in_src)
{
    // this can be more efficient, but for now it'll do!
    for (size_t s = 0; s < in_src->count; s++)
    {
        array_append(in_dest, in_src->items[s]);
    }
}


/*
size_t array_index_of(Array *in_array, void *in_item)
{
    if (in_array->count == 0) return ARRAY_INVALID_INDEX;
    if (in_array->count < 2)
    {
        if (in_array->items[0] == in_item) return 0;
        else return ARRAY_INVALID_INDEX;
    }
    size_t search_range_start = 0;
    size_t search_range_end = in_array->count - 1;
    while (search_range_end - search_range_start > 1)
    {
        size_t range_middle_index = (search_range_end - search_range_start) / 2;
        if (range_)
    }
}
 */


void array_enumerate(Array *in_array, void* in_reserved, void *in_reserved2)
{
    in_array->iterator_index = 0;
}


void* array_iterate(Array *in_array)
{
    in_array->iterator_index = 0;
    return NULL;
}

void* array_next(Array *in_array)
{
    if (in_array->iterator_index >= in_array->count) return NULL;
    return in_array->items[in_array->iterator_index++];
}




