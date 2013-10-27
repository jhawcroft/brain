//
//  set.c
//  AI-NLPartA-IdeaA
//
//  Created by Joshua Hawcroft on 12/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "set.h"


#if SET_COMPILE_SELF_TESTS == 1
#define CHECK_INTERNAL_CONSISTENCY 1
#else
#define CHECK_INTERNAL_CONSISTENCY 0
#endif


#define SET_INVALID_INDEX 0xFFFFFFFF


struct Set_
{
    void **items;
    unsigned long count;
};


Set* set_create(void)
{
    Set *result = calloc(1, sizeof(Set));
    return result;
}


void set_dispose(Set *in_set)
{
    if (in_set->items) free(in_set->items);
    free(in_set);
}


#if DEBUG && (CHECK_INTERNAL_CONSISTENCY == 1)
static void internal_consistency_check_(Set *in_set)
{
    void *last = NULL;
    for (unsigned long i = 0; i < in_set->count; i++)
    {
        if (in_set->items[i] > last)
            last = in_set->items[i];
        else
        {
            fprintf(stderr, "set: internal_consistency_check_(): failed");
            abort();
        }
    }
}
#endif


static unsigned long get_appropriate_index_(Set *in_set, void *in_item)
{
    if (in_set->count == 0) return 0;
    if (in_set->count == 1)
    {
        if (in_set->items[0] == in_item) return 0;
        else if (in_set->items[0] > in_item) return 0;
        else return 1;
    }
    unsigned long range_start = 0;
    unsigned long range_end = in_set->count - 1;
    while (range_end - range_start > 0)
    {
        unsigned long range_middle = (range_end - range_start) / 2 + range_start;
        void *middle_item = in_set->items[range_middle];
        if (in_item < middle_item)
            range_end = range_middle;
        else if (in_item == middle_item)
            return range_middle;
        else
            range_start = range_middle + 1;
    }
    if (in_set->items[range_start] < in_item) return range_start + 1;
    else return range_start;
}


void set_add(Set *in_set, void *in_item)
{
    unsigned long existing_index = get_appropriate_index_(in_set, in_item);
    if ((existing_index < in_set->count) &&
        (in_set->items[existing_index] == in_item)) return;
    
    void **new_items = realloc(in_set->items, sizeof(void*) * (in_set->count + 1));
    if (!new_items)
    {
        fprintf(stderr, "set_add(): failed to allocate memory\n");
        abort();
    }
    in_set->items = new_items;
    
    memmove(&(in_set->items[existing_index + 1]), &(in_set->items[existing_index]),
            sizeof(void*) * (in_set->count - existing_index));
    in_set->count++;
    in_set->items[existing_index] = in_item;
    
#if DEBUG && (CHECK_INTERNAL_CONSISTENCY == 1)
    internal_consistency_check_(in_set);
#endif
}


static void set_remove_(Set *in_set, unsigned long in_index)
{
    memmove(&(in_set->items[in_index]), &(in_set->items[in_index + 1]),
            sizeof(void*) * (in_set->count - in_index - 1));
    in_set->count--;
}


void set_remove(Set *in_set, void *in_item)
{
    unsigned long existing_index = get_appropriate_index_(in_set, in_item);
    if ((existing_index >= in_set->count) ||
        (in_set->items[existing_index] != in_item)) return;
    
    set_remove_(in_set, existing_index);
    
#if DEBUG && (CHECK_INTERNAL_CONSISTENCY == 1)
    internal_consistency_check_(in_set);
#endif
}


int set_contains(Set *in_set, void *in_item)
{
    unsigned long existing_index = get_appropriate_index_(in_set, in_item);
    return ((existing_index < in_set->count) &&
            (in_set->items[existing_index] == in_item));
}


int set_is_empty(Set *in_set)
{
    return (in_set->count == 0);
}


unsigned long set_count(Set *in_set)
{
    return in_set->count;
}


void* set_item(Set *in_set, unsigned long in_index)
{
    return in_set->items[in_index];
}


void set_union(Set *in_dest, Set *in_src)
{
    for (unsigned long i = 0; i < in_src->count; i++)
    {
        set_add(in_dest, in_src->items[i]);
    }
}


void set_intersection(Set *in_dest, Set *in_src)
{
    for (unsigned long i = 0; i < in_dest->count; i++)
    {
        void *item = in_dest->items[i];
        if (!set_contains(in_src, item))
        {
            set_remove_(in_dest, i);
            i--;
        }
    }
}


void set_difference(Set *in_dest, Set *in_src)
{
    for (unsigned long i = 0; i < in_src->count; i++)
    {
        set_remove(in_dest, in_src->items[i]);
    }
}








