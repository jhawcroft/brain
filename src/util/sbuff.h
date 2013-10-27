//
//  stringbuff.h
//  PrototypeNLUnit
//
//  Created by Joshua Hawcroft on 19/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

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
