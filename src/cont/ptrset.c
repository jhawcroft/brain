/*
 PtrSet
 v. 1.1
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 (see header for description)
 
 ----------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ptrset.h"


#if PTRSET_COMPILE_SELF_TESTS == 1
#define CHECK_INTERNAL_CONSISTENCY 1
#else
#define CHECK_INTERNAL_CONSISTENCY 0
#endif


#define PTRSET_INVALID_INDEX 0xFFFFFFFF


struct PtrSet_
{
    void **items;
    unsigned long count;
    PtrSetCBDisposeItem dis_item;
    void *context;
};


PtrSet* ptrset_create(void *in_context, PtrSetCBDisposeItem in_dis_item)
{
    PtrSet *result = calloc(1, sizeof(PtrSet));
    result->context = in_context;
    result->dis_item = in_dis_item;
    return result;
}


void ptrset_dispose(PtrSet *in_set)
{
    if (in_set->dis_item)
    {
        for (int i = 0; i < in_set->count; i++)
            in_set->dis_item(in_set->context, in_set->items[i]);
    }
    if (in_set->items) free(in_set->items);
    free(in_set);
}


#if DEBUG && (CHECK_INTERNAL_CONSISTENCY == 1)
static void internal_consistency_check_(PtrSet *in_set)
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


static unsigned long get_appropriate_index_(PtrSet *in_set, void *in_item)
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


void* ptrset_add(PtrSet *in_set, void *in_item)
{
    unsigned long existing_index = get_appropriate_index_(in_set, in_item);
    if ((existing_index < in_set->count) &&
        (in_set->items[existing_index] == in_item)) return NULL;
    
    void **new_items = realloc(in_set->items, sizeof(void*) * (in_set->count + 1));
    if (!new_items) return NULL;
    in_set->items = new_items;
    
    memmove(&(in_set->items[existing_index + 1]), &(in_set->items[existing_index]),
            sizeof(void*) * (in_set->count - existing_index));
    in_set->count++;
    in_set->items[existing_index] = in_item;
    
#if DEBUG && (CHECK_INTERNAL_CONSISTENCY == 1)
    internal_consistency_check_(in_set);
#endif
    
    return in_item;
}


static void ptrset_remove_(PtrSet *in_set, unsigned long in_index)
{
    if (in_set->dis_item)
        in_set->dis_item(in_set->context, in_set->items[in_index]);
    
    memmove(&(in_set->items[in_index]), &(in_set->items[in_index + 1]),
            sizeof(void*) * (in_set->count - in_index - 1));
    in_set->count--;
}


void ptrset_remove(PtrSet *in_set, void *in_item)
{
    unsigned long existing_index = get_appropriate_index_(in_set, in_item);
    if ((existing_index >= in_set->count) ||
        (in_set->items[existing_index] != in_item)) return;
    
    ptrset_remove_(in_set, existing_index);
    
#if DEBUG && (CHECK_INTERNAL_CONSISTENCY == 1)
    internal_consistency_check_(in_set);
#endif
}


int ptrset_contains(PtrSet *in_set, void *in_item)
{
    unsigned long existing_index = get_appropriate_index_(in_set, in_item);
    return ((existing_index < in_set->count) &&
            (in_set->items[existing_index] == in_item));
}


int ptrset_is_empty(PtrSet *in_set)
{
    return (in_set->count == 0);
}


unsigned long ptrset_count(PtrSet *in_set)
{
    return in_set->count;
}


void* ptrset_item(PtrSet *in_set, unsigned long in_index)
{
    return in_set->items[in_index];
}


void ptrset_union(PtrSet *in_dest, PtrSet *in_src)
{
    for (unsigned long i = 0; i < in_src->count; i++)
    {
        ptrset_add(in_dest, in_src->items[i]);
    }
}


void ptrset_intersection(PtrSet *in_dest, PtrSet *in_src)
{
    for (unsigned long i = 0; i < in_dest->count; i++)
    {
        void *item = in_dest->items[i];
        if (!ptrset_contains(in_src, item))
        {
            ptrset_remove_(in_dest, i);
            i--;
        }
    }
}


void ptrset_difference(PtrSet *in_dest, PtrSet *in_src)
{
    for (unsigned long i = 0; i < in_src->count; i++)
    {
        ptrset_remove(in_dest, in_src->items[i]);
    }
}








