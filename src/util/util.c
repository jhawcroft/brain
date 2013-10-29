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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "alloc.h"


char* brain_strdup(char const *in_string)
{
    if (!in_string) in_string = "";
    long length = strlen(in_string);
    char *result = brain_alloc_(length + 1, 0);
    if (!result) return NULL;
    strcpy(result, in_string);
    return result;
}


/* TODO: needs to be altered to use GNU libunistring / ICS */
char* brain_strdup_upper(char const *in_string)
{
    if (!in_string) in_string = "";
    long length = strlen(in_string);
    char *result = brain_alloc_(length + 1, 0);
    if (!result) return NULL;
    result[length] = 0;
    for (int i = 0; i < length; i++)
    {
        result[i] = toupper(in_string[i]);
    }
    return result;
}




