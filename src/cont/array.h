/*
 Array
 v. 1.1
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 Simple randomly-accessible dynamic array.
 
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

#ifndef JWH_ARRAY_H
#define JWH_ARRAY_H

#include <stdlib.h>


typedef struct Array_ Array;

#define ARRAY_INVALID_INDEX 0xFFFFFFFF


enum
{
    ARRAY_OK = 0,
    ARRAY_EMEM,
};


typedef void (*ArrayCBDisposeItem) (void *in_context, void *in_item);


Array* array_create(void *in_context, ArrayCBDisposeItem in_dis_item);
void array_dispose(Array *in_array);

Array* array_clone(Array *in_array);

void* array_insert(Array *in_array, size_t in_at_index, void *in_item);
void* array_append(Array *in_array, void *in_item);
int array_remove(Array *in_array, size_t in_index, void **out_item);
size_t array_count(Array *in_array);
void* array_item(Array *in_array, size_t in_index);

void* array_iterate(Array *in_array);
void* array_next(Array *in_array);

void array_union(Array *in_dest, Array *in_src);
void array_intersect(Array *in_dest, Array *in_src);


#endif
