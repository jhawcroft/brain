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
#include <string.h>

#include "sbuff.h"


struct Sbuff_
{
    char *buff;
    size_t length;
};


Sbuff* sbuff_create(size_t in_capacity)
{
    Sbuff *result = calloc(1, sizeof(Sbuff));
    result->buff = calloc(1, 1);
    if (!result->buff)
    {
        free(result);
        return NULL;
    }
    return result;
}


void sbuff_dispose(Sbuff *in_sbuff)
{
    if (in_sbuff->buff) free(in_sbuff->buff);
    free(in_sbuff);
}


int sbuff_append_cstring(Sbuff *in_sbuff, char const *in_string, size_t in_length)
{
    if (in_length == SBUFF_AUTO_LENGTH) in_length = strlen(in_string);
    
    // very inefficient string buffer!
    char *new_buff = realloc(in_sbuff->buff, in_sbuff->length + in_length + 1);
    if (!new_buff) return SBUFF_ERROR_MEMORY;
    
    in_sbuff->buff = new_buff;
    memcpy(new_buff + in_sbuff->length, in_string, in_length);
    in_sbuff->length += in_length;
    new_buff[in_sbuff->length] = 0;
    
    return SBUFF_NO_ERROR;
}


int sbuff_append_cstring_disposing(Sbuff *in_sbuff, char *in_string, size_t in_length)
{
    int result = sbuff_append_cstring(in_sbuff, in_string, in_length);
    free(in_string);
    return result;
}


char const* sbuff_cstring(Sbuff *in_sbuff)
{
    return (char const*)in_sbuff->buff;
}


char* sbuff_cstring_disposing(Sbuff *in_sbuff)
{
    char *result = in_sbuff->buff;
    free(in_sbuff);
    return result;
}


