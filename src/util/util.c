//
//  util.c
//  PrototypeNLUnit
//
//  Created by Joshua Hawcroft on 13/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util.h"


char* jh_strdup(char const *in_string)
{
    if (!in_string) in_string = "";
    long length = strlen(in_string);
    char *result = malloc(length + 1);
    if (!result) return NULL;
    strcpy(result, in_string);
    return result;
}


/* needs to be altered to use GNU libunistring / ICS */
char* strdup_to_upper(char const *in_string)
{
    if (!in_string) in_string = "";
    long length = strlen(in_string);
    char *result = malloc(length + 1);
    if (!result) return NULL;
    result[length] = 0;
    for (int i = 0; i < length; i++)
    {
        result[i] = toupper(in_string[i]);
    }
    return result;
}



char int_ptr_vals[MAX_INT_PTR + 1];


