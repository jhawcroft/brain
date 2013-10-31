/*
 
 Brain Rarely Accepts Incoherent Nonsense (BRAIN)
 Copyright 2012-2013 Joshua Hawcroft
 
 This file is part of BRAIN.
 
 BRAIN is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 BRAIN is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with BRAIN.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include <stdlib.h>

#include "alloc.h"

/* TODO: ensure that when we implement an optimized, managed allocator,
 that utilities such as thought continue to operate with the basic allocator,
 probably hint 0 */

void* brain_alloc_(size_t in_size, int in_hint)
{
    return malloc(in_size);
}


void* brain_realloc_(void *in_mem, size_t in_size, int in_hint)
{
    return realloc(in_mem, in_size);
}


void brain_free_(void *in_mem)
{
    free(in_mem);
}



