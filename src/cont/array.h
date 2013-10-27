//
//  array.h
//  AI-NLPartA-IdeaA
//
//  Created by Joshua Hawcroft on 12/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#ifndef AI_NLPartA_IdeaA_array_h
#define AI_NLPartA_IdeaA_array_h

#include <stdlib.h>


typedef struct Array_ Array;

#define ARRAY_INVALID_INDEX 0xFFFFFFFF


Array* array_create(void);
void array_dispose(Array *in_array);

Array* array_clone(Array *in_array);

void array_insert(Array *in_array, size_t in_at_index, void *in_item);
void array_append(Array *in_array, void *in_item);
void array_remove(Array *in_array, size_t in_index);
size_t array_count(Array *in_array);
void* array_item(Array *in_array, size_t in_index);

void* array_iterate(Array *in_array);
void* array_next(Array *in_array);

void array_union(Array *in_dest, Array *in_src);
void array_intersect(Array *in_dest, Array *in_src);


#endif
