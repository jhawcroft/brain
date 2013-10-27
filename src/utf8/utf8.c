//
//  utf8.c
//  PrototypeNLUnit
//
//  Created by Joshua Hawcroft on 17/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uninorm.h>
#include <unicase.h>

#include "hashmap.h"


int hashmap_utf8_compare_(void *in_context, void *in_left, void *in_right)
{
    int result = 0;
    //printf("Compare UTF8: \"%s\" and \"%s\"\n", in_left, in_right);
    u8_casecmp((const unistring_uint8_t*)in_left, strlen(in_left),
               (const unistring_uint8_t*)in_right, strlen(in_right),
               uc_locale_language(), UC_DECOMP_CANONICAL, &result);
    return result;
}


long hashmap_utf8_hash_(void *in_context, void *in_key)
{
    //printf("Hash UTF8: \"%s\"\n", in_key);
    size_t length = 254;
    unistring_uint8_t buffer[255]; /* going to use this for now, as we can work with existing hash function more easily;
                                    as there is a NULL terminator */
    uint8_t *case_folded = u8_ct_casefold((const unistring_uint8_t*)in_key, strlen(in_key),
                                          unicase_empty_prefix_context,
                                          unicase_empty_suffix_context,
                                          uc_locale_language(), UC_DECOMP_CANONICAL,
                                          buffer, &length);
    buffer[length] = 0;
    long result = 0;
    //if (case_folded)
    //{
    result = hashmap_hash_cstring(in_context, buffer);
    //    free(case_folded);
    //}
    return result;
}


