//
//  fileread.c
//  PrototypeNLUnit
//
//  Created by Joshua Hawcroft on 13/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file-read.h"


#define READ_BUFFER_BYTES 4096


char* file_read(char const *in_filepath)
{
    char *result = NULL;
    long result_size = 0;
    char buffer[READ_BUFFER_BYTES];
    long bytes_read;
    
    FILE *fp = fopen(in_filepath, "r");
    if (!fp) return NULL;
    
    while ( (bytes_read = fread(buffer, 1, READ_BUFFER_BYTES, fp)) > 0 )
    {
        char *new_result = realloc(result, result_size + bytes_read + 1);
        if (!new_result)
        {
            if (result) free(result);
            return NULL;
        }
        result = new_result;
        memcpy(result + result_size, buffer, bytes_read);
        result_size += bytes_read;
    }
    
    fclose(fp);
    
    result[result_size] = 0;
    return result;
}






