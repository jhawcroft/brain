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
/*
 String buffer - efficient string concatenation
 */

#ifndef PrototypeNLUnit_stringbuff_h
#define PrototypeNLUnit_stringbuff_h

#include <stdlib.h>

enum {
    SBUFF_NO_ERROR = 0,
    SBUFF_ERROR_MEMORY,
};


#define SBUFF_DEFAULT_CAPACITY 0
#define SBUFF_AUTO_LENGTH 0xFFFFFFFF


typedef struct Sbuff_ Sbuff;

Sbuff* sbuff_create(size_t in_capacity);
void sbuff_dispose(Sbuff *in_sbuff);

int sbuff_append_cstring(Sbuff *in_sbuff, char const *in_string, size_t in_length);
int sbuff_append_cstring_disposing(Sbuff *in_sbuff, char *in_string, size_t in_length);

char const* sbuff_cstring(Sbuff *in_sbuff);
char* sbuff_cstring_disposing(Sbuff *in_sbuff);


#endif
