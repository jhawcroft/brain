/*
 Hashmap
 v. 1.1
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 (See the header file hashmap.h for a description and configuration)
 
 ----------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h> /* strcmp used in default string comparator */

#include "hashmap.h"


/*
 Compile-time Configuration
 */

/* how many empty pairs to pre-allocate when the bucket is full? */
#define PAIR_ALLOC 5


/*
 Internal Structures
 */

typedef struct Pair_
{
    void *key;
    void *item;
    
} Pair;


typedef struct Bucket_
{
    unsigned long pair_count;
    unsigned long pair_alloc;
    Pair **pairs; /* sorted by key */
    
} Bucket;


struct Hashmap_
{
    int bucket_count;
    Bucket *buckets;
    unsigned long item_count;
    void *context;
    HashmapCBHash cb_hash;
    HashmapCBCompare cb_compare;
    HashmapCBDisposeKey cb_dis_key;
    HashmapCBDisposeValue cb_dis_value;
};



/*
 Implementation
 */


#define IFSAI_INDEX_TYPE unsigned long
static IFSAI_INDEX_TYPE ifsai(void *in_list, void *in_item,
                       IFSAI_INDEX_TYPE (*in_func_count) (void *in_list),
                       int (*in_func_compare) (void *in_list, void *in_left, void *in_right),
                       void* (*in_func_get) (void *in_list, IFSAI_INDEX_TYPE in_index))
{
    IFSAI_INDEX_TYPE count = in_func_count(in_list);
    if (count == 0) return 0;
    IFSAI_INDEX_TYPE range_start = 0;
    IFSAI_INDEX_TYPE range_end = count - 1;
    while (range_end - range_start > 0)
    {
        IFSAI_INDEX_TYPE range_middle = (range_end - range_start) / 2 + range_start;
        void *middle_item = in_func_get(in_list, range_middle);
        int comp = in_func_compare(in_list, in_item, middle_item);
        if (comp < 0) range_end = range_middle;
        else if (comp == 0) return range_middle;
        else range_start = range_middle + 1;
    }
    int comp = in_func_compare(in_list, in_func_get(in_list, range_start), in_item);
    if (comp < 0) return range_start + 1;
    else return range_start;
}



Hashmap* hashmap_create(int in_buckets, void *in_context,
                        HashmapCBHash in_hash_func, HashmapCBCompare in_compare_func,
                        HashmapCBDisposeKey in_dis_key, HashmapCBDisposeValue in_dis_value)
{
    Hashmap *result = calloc(1, sizeof(Hashmap));
    if (!result) return NULL;
    
    result->bucket_count = in_buckets;
    result->buckets = calloc(in_buckets, sizeof(Bucket));
    if (!result->buckets)
    {
        free(result);
        return NULL;
    }
    
    result->context = in_context;
    result->cb_hash = in_hash_func;
    result->cb_compare = in_compare_func;
    result->cb_dis_key = in_dis_key;
    result->cb_dis_value = in_dis_value;
    
    return result;
}


void hashmap_dispose(Hashmap *in_hashmap)
{
    for (int b = 0; b < in_hashmap->bucket_count; b++)
    {
        Bucket *bucket = &(in_hashmap->buckets[b]);
        for (unsigned long p = 0; p < bucket->pair_count; p++)
        {
            if (in_hashmap->cb_dis_value)
                in_hashmap->cb_dis_value(in_hashmap->context, bucket->pairs[p]->key, bucket->pairs[p]->item);
            if ((in_hashmap->cb_dis_key) && (bucket->pairs[p]->key))
                in_hashmap->cb_dis_key(in_hashmap->context, bucket->pairs[p]->key);
            free(bucket->pairs[p]);
        }
    }
    if (in_hashmap->buckets) free(in_hashmap->buckets);
    free(in_hashmap);
}


static unsigned long ifsai_count_(Bucket *in_bucket)
{
    return in_bucket->pair_count;
}


static void* ifsai_item_(Bucket *in_bucket, unsigned long in_index)
{
    return in_bucket->pairs[in_index]->key;
}


static unsigned long pair_index_(Hashmap *in_hashmap, void *in_key, Bucket **out_bucket)
{
    int bucket_index = (unsigned long)(in_hashmap->cb_hash(in_hashmap->context, in_key)) % in_hashmap->bucket_count;
    Bucket *bucket = &(in_hashmap->buckets[bucket_index]);
    *out_bucket = bucket;
    return ifsai(bucket,
                 in_key,
                 (unsigned long(*)(void*))&ifsai_count_,
                 in_hashmap->cb_compare,
                 (void*(*)(void*, unsigned long))&ifsai_item_);
}


#define PAIR_MATCHES ((pair_index != bucket->pair_count) && \
(in_hashmap->cb_compare(in_hashmap->context, bucket->pairs[pair_index]->key, in_key) == 0))


int hashmap_add(Hashmap *in_hashmap, void *in_key, void *in_item)
{
    int result;
    Bucket *bucket;
    Pair *pair;
    unsigned long pair_index = pair_index_(in_hashmap, in_key, &bucket);
    if (!PAIR_MATCHES)
    {
        if (bucket->pair_count + 1 > bucket->pair_alloc)
        {
            Pair **new_pairs = realloc(bucket->pairs, sizeof(Pair*) * (bucket->pair_alloc + PAIR_ALLOC));
            if (!new_pairs) return HASHMAP_ERROR_MEMORY;
            bucket->pairs = new_pairs;
            bucket->pair_alloc += PAIR_ALLOC;
        }
        memmove(&(bucket->pairs[pair_index + 1]), &(bucket->pairs[pair_index]), sizeof(Pair*) * (bucket->pair_count - pair_index));
        result = HASHMAP_NOT_FOUND;
        pair = malloc(sizeof(Pair));
        if (!pair) return HASHMAP_ERROR_MEMORY;
        bucket->pairs[pair_index] = pair;
        bucket->pair_count++;
        in_hashmap->item_count++;
    }
    else
    {
        result = HASHMAP_FOUND;
        pair = bucket->pairs[pair_index];
    }
    pair->key = in_key;
    pair->item = in_item;
    return result;
}


int hashmap_remove(Hashmap *in_hashmap, void *in_key, void **out_key, void **out_value)
{
    Bucket *bucket;
    unsigned long pair_index = pair_index_(in_hashmap, in_key, &bucket);
    if (!PAIR_MATCHES) return HASHMAP_NOT_FOUND;
    
    if (out_value) *out_value = bucket->pairs[pair_index]->item;
    if (out_key) *out_key = bucket->pairs[pair_index]->key;
    
    if (in_hashmap->cb_dis_value)
    {
        in_hashmap->cb_dis_value(in_hashmap->context, bucket->pairs[pair_index]->key,
                                 bucket->pairs[pair_index]->item);
        if (out_value) *out_value = NULL;
    }
    if ((in_hashmap->cb_dis_key) && (bucket->pairs[pair_index]->key))
    {
        in_hashmap->cb_dis_key(in_hashmap->context, bucket->pairs[pair_index]->key);
        if (out_key) *out_key = NULL;
    }
    
    free(bucket->pairs[pair_index]);
    memmove(&(bucket->pairs[pair_index]), &(bucket->pairs[pair_index + 1]), sizeof(Pair*) * (bucket->pair_count - pair_index - 1));
    bucket->pair_count--;
    in_hashmap->item_count--;
    return HASHMAP_FOUND;
}


void* hashmap_item(Hashmap *in_hashmap, void *in_key)
{
    Bucket *bucket;
    unsigned long pair_index = pair_index_(in_hashmap, in_key, &bucket);
    if (!PAIR_MATCHES) return NULL;
    return bucket->pairs[pair_index]->item;
}


void hashmap_clear(Hashmap *in_hashmap)
{
    for (int b = 0; b < in_hashmap->bucket_count; b++)
    {
        Bucket *bucket = &(in_hashmap->buckets[b]);
        for (unsigned long p = 0; p < bucket->pair_count; p++)
        {
            if (in_hashmap->cb_dis_value)
                in_hashmap->cb_dis_value(in_hashmap->context, bucket->pairs[p]->key, bucket->pairs[p]->item);
            if ((in_hashmap->cb_dis_key) && (bucket->pairs[p]->key))
                in_hashmap->cb_dis_key(in_hashmap->context, bucket->pairs[p]->key);
            free(bucket->pairs[p]);
        }
        bucket->pair_count = 0;
    }
    in_hashmap->item_count = 0;
}


int hashmap_contains(Hashmap *in_hashmap, void *in_key)
{
    Bucket *bucket;
    unsigned long pair_index = pair_index_(in_hashmap, in_key, &bucket);
    if (!PAIR_MATCHES) return HASHMAP_NOT_FOUND;
    else return HASHMAP_FOUND;
}


unsigned long hashmap_count(Hashmap *in_hashmap)
{
    return in_hashmap->item_count;
}


void hashmap_enumerate(Hashmap *in_hashmap, void (*in_enumerator) (void *in_context, void *in_key, void *in_item))
{
    for (int b = 0; b < in_hashmap->bucket_count; b++)
    {
        Bucket *bucket = &(in_hashmap->buckets[b]);
        for (unsigned long p = 0; p < bucket->pair_count; p++)
        {
            in_enumerator(in_hashmap->context, bucket->pairs[p]->key, bucket->pairs[p]->item);
        }
    }
}



/* Convenience Default Implementations */


/* hash function for c-strings */
long hashmap_hash_cstring(void *in_context, void *in_key)
{
    /* implements 'djb2' by Dan Bernstein */
    unsigned long hash = 5381;
    int c;
    while ( (c = *((unsigned char*)in_key++)) )
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}


/* case-sensitive comparison function for c-strings */
int hashmap_compare_cstring(void *in_context, void *in_left, void *in_right)
{
    return strcmp(in_left, in_right);
}



/*
 Tests
 */

#if COMPILE_HASHMAP_TEST_ == 1

#include <stdio.h>
#include <assert.h>


static int g_test_counter = 0;

static char const *pickle_value = "PICKLES";
static char const *apple_value = "APPLES";
static char const *pizza_value = "PIZZA";


static void test_enumerator_(void *in_context, void *in_key, void *in_item)
{
    switch (g_test_counter++)
    {
        case 0:
            assert(strcmp(in_key, "pickles") == 0);
            assert(in_item == pickle_value);
            break;
        case 2:
            assert(strcmp(in_key, "apples") == 0);
            assert(in_item == apple_value);
            break;
        case 1:
            assert(strcmp(in_key, "pizza") == 0);
            assert(in_item == pizza_value);
            break;
    }
}


void hashmap_test(void)
{
    printf("hashmap: running tests...");
    
    Hashmap *map_1;
    
    map_1 = hashmap_create(100, NULL, &hashmap_hash_cstring, &hashmap_compare_cstring);
    hashmap_add(map_1, "pickles", (void*)pickle_value);
    assert( hashmap_count(map_1) == 1 );
    
    assert( hashmap_contains(map_1, "apricots") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pickles") == HASHMAP_FOUND );
    
    assert( hashmap_item(map_1, "apricots") == NULL );
    assert( hashmap_item(map_1, "pickles") == pickle_value );
    
    hashmap_add(map_1, "pickles", (void*)pickle_value);
    assert( hashmap_count(map_1) == 1 );
    
    hashmap_add(map_1, "apples", (void*)apple_value);
    assert( hashmap_count(map_1) == 2 );
    
    assert( hashmap_contains(map_1, "apricots") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pickles") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "apples") == HASHMAP_FOUND );
    
    assert( hashmap_item(map_1, "apricots") == NULL );
    assert( hashmap_item(map_1, "pickles") == pickle_value );
    assert( hashmap_item(map_1, "apples") == apple_value );
    
    hashmap_add(map_1, "pizza", (void*)pizza_value);
    assert( hashmap_count(map_1) == 3 );
    
    assert( hashmap_contains(map_1, "apricots") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pickles") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "apples") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "pizza") == HASHMAP_FOUND );
    
    assert( hashmap_item(map_1, "apricots") == NULL );
    assert( hashmap_item(map_1, "pickles") == pickle_value );
    assert( hashmap_item(map_1, "apples") == apple_value );
    assert( hashmap_item(map_1, "pizza") == pizza_value );
    
    assert(map_1->buckets[28].pair_count == 1);
    assert(map_1->buckets[66].pair_count == 1);
    assert(map_1->buckets[51].pair_count == 1);
    assert(map_1->buckets[10].pair_count == 0);
    
    hashmap_enumerate(map_1, test_enumerator_);
    
    hashmap_remove(map_1, "apples");
    assert( hashmap_count(map_1) == 2 );
    
    assert( hashmap_contains(map_1, "apricots") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pickles") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "apples") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pizza") == HASHMAP_FOUND );
    
    assert( hashmap_item(map_1, "apricots") == NULL );
    assert( hashmap_item(map_1, "pickles") == pickle_value );
    assert( hashmap_item(map_1, "apples") == NULL );
    assert( hashmap_item(map_1, "pizza") == pizza_value );
    
    hashmap_clear(map_1);
    assert( hashmap_count(map_1) == 0 );
    
    hashmap_dispose(map_1);
    
    assert( (unsigned long)hashmap_hash_cstring(NULL, "pickles") % 100 == 28 );
    assert( (unsigned long)hashmap_hash_cstring(NULL, "apples") % 100 == 66 );
    assert( (unsigned long)hashmap_hash_cstring(NULL, "pizza") % 100 == 51 );
    assert( (unsigned long)hashmap_hash_cstring(NULL, "apricots") % 100 == 10 );
    
    map_1 = hashmap_create(1, NULL, &hashmap_hash_cstring, &hashmap_compare_cstring);
    hashmap_add(map_1, "pickles", (void*)pickle_value);
    assert( hashmap_count(map_1) == 1 );
    hashmap_add(map_1, "pizza", (void*)pizza_value);
    assert( hashmap_count(map_1) == 2 );
    hashmap_add(map_1, "apples", (void*)apple_value);
    assert( hashmap_count(map_1) == 3 );
    
    assert( hashmap_contains(map_1, "apricots") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pickles") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "apples") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "pizza") == HASHMAP_FOUND );
    
    assert( hashmap_item(map_1, "apricots") == NULL );
    assert( hashmap_item(map_1, "pickles") == pickle_value );
    assert( hashmap_item(map_1, "apples") == apple_value );
    assert( hashmap_item(map_1, "pizza") == pizza_value );
    
    assert(map_1->buckets[0].pair_count == 3);
    
    hashmap_add(map_1, "pizza", (void*)apple_value);
    assert( hashmap_count(map_1) == 3 );
    
    assert( hashmap_contains(map_1, "apricots") == HASHMAP_NOT_FOUND );
    assert( hashmap_contains(map_1, "pickles") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "apples") == HASHMAP_FOUND );
    assert( hashmap_contains(map_1, "pizza") == HASHMAP_FOUND );
    
    assert( hashmap_item(map_1, "apricots") == NULL );
    assert( hashmap_item(map_1, "pickles") == pickle_value );
    assert( hashmap_item(map_1, "apples") == apple_value );
    assert( hashmap_item(map_1, "pizza") == apple_value );
    
    assert(strcmp(map_1->buckets[0].pairs[0]->key, "apples") == 0);
    assert(strcmp(map_1->buckets[0].pairs[1]->key, "pickles") == 0);
    assert(strcmp(map_1->buckets[0].pairs[2]->key, "pizza") == 0);
    
    hashmap_dispose(map_1);
    
    printf("ok.\n");
}


int main(int argc, char *argv[])
{
    hashmap_test();
    return 0;
}


#endif










