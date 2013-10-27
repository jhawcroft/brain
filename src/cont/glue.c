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

#include "glue.h"
#include "../mem/alloc.h"



void cont_dispose_key_(void *in_context, void *in_key)
{
    if (in_key)
        brain_free_(in_key);
}


void cont_dispose_key_value_(void *in_context, void *in_key, void *in_value)
{
    if (in_value)
        brain_free_(in_value);
}


void cont_dispose_item_(void *in_context, void *in_item)
{
    if (in_item)
        brain_free_(in_item);
}


