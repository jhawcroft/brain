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

#if USE_GNU_LIBUNISTRING == 1
#include <uninorm.h>
#include <unicase.h>

#else
#include <string.h>
#include <ctype.h>
#endif

#include "../cont/hashmap.h"


#if USE_GNU_LIBUNISTRING == 1

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


#else /* use ASCII string utilities if not configured otherwise... */


int hashmap_utf8_compare_(void *in_context, void *in_left, void *in_right)
{
    return strcmp(in_left, in_right);
}


long hashmap_utf8_hash_(void *in_context, void *in_key)
{
    return hashmap_hash_cstring(in_context, in_key);
}


#endif




