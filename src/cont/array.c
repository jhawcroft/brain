/*
 Array
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

#include "array.h"



struct Array_
{
    void **items;
    size_t count;
    size_t iterator_index;
    void *context;
    ArrayCBDisposeItem dis_item;
};



Array* array_create(void *in_context, ArrayCBDisposeItem in_dis_item)
{
    Array *array = calloc(1, sizeof(Array));
    array->context = in_context;
    array->dis_item = in_dis_item;
    return array;
}


Array* array_clone(Array *in_array)
{
    Array *array = array_create(in_array->context, in_array->dis_item);
    if (in_array->count > 0)
        array->items = malloc(sizeof(void*) * in_array->count);
    array->count = in_array->count;
    memcpy(array->items, in_array->items, sizeof(void*) * (in_array->count));
    return array;
}


void array_dispose(Array *in_array)
{
    if (in_array->dis_item)
    {
        for (int i = 0; i < in_array->count; i++)
            in_array->dis_item(in_array->context, in_array->items[i]);
    }
    if (in_array->items) free(in_array->items);
    free(in_array);
}


void* array_insert(Array *in_array, size_t in_at_index, void *in_item)
{
    void **new_items = realloc(in_array->items, sizeof(void*) * (in_array->count + 1));
    if (!new_items) return NULL;
    in_array->items = new_items;
    if (! memmove(&(new_items[in_at_index + 1]), &(new_items[in_at_index]), sizeof(void*) * (in_array->count - in_at_index)) ) return NULL;
    new_items[in_at_index] = in_item;
    in_array->count++;
    return in_item;
}


void* array_append(Array *in_array, void *in_item)
{
    return array_insert(in_array, in_array->count, in_item);
}


int array_remove(Array *in_array, size_t in_index, void **out_item)
{
    void *item = in_array->items[in_index];
    
    if (in_array->dis_item)
    {
        in_array->dis_item(in_array->context, in_array->items[in_index]);
        item = NULL;
    }
    
    if (! memmove(&(in_array->items[in_index]), &(in_array->items[in_index + 1]), sizeof(void*) * (in_array->count - in_index - 1)) ) return ARRAY_EMEM;
    in_array->count--;
    
    if (out_item) *out_item = item;
    return ARRAY_OK;
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
        (void)array_remove(in_dest, d, NULL);
    intersection_ds_found_item:
        continue;
    }
}


void array_union(Array *in_dest, Array *in_src)
{
    /* this can be more efficient, but for now it'll do! */
    for (size_t s = 0; s < in_src->count; s++)
    {
        array_append(in_dest, in_src->items[s]);
    }
}


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


size_t array_index_of(Array *in_array, void *in_item)
{
    for (int i = 0; i < in_array->count; i++)
    {
        if (in_array->items[i] == in_item) return i;
    }
    return ARRAY_INVALID_INDEX;
}



