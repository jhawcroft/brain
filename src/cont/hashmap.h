/*
 Hashmap
 v. 1.1
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 Hashmap implementation for rapid association of keys with items.
 
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

#ifndef JHW_HASHMAP_H
#define JHW_HASHMAP_H

/* Types */

typedef struct Hashmap_ Hashmap;

typedef long (*HashmapCBHash) (void *in_context, void *in_key);
typedef int (*HashmapCBCompare) (void *in_context, void *in_left, void *in_right);
typedef void (*HashmapCBDisposeKey) (void *in_context, void *in_key);
typedef void (*HashmapCBDisposeValue) (void *in_context, void *in_key, void *in_value);


/* Constants */

enum {
    HASHMAP_FOUND = 0,
    HASHMAP_NOT_FOUND,
    HASHMAP_ERROR_MEMORY,
};


/* Convenience Macros */

#define HASHMAP_OK(inStatus) (inStatus < HASHMAP_ERROR_MEMORY)


/* API */

Hashmap* hashmap_create(int in_buckets, void *in_context,
                        HashmapCBHash in_hash_func, HashmapCBCompare in_compare_func,
                        HashmapCBDisposeKey in_dis_key, HashmapCBDisposeValue in_dis_value);
void hashmap_dispose(Hashmap *in_hashmap);

int hashmap_add(Hashmap *in_hashmap, void *in_key, void *in_item);
int hashmap_remove(Hashmap *in_hashmap, void *in_key, void **out_key, void **out_value);
void* hashmap_item(Hashmap *in_hashmap, void *in_key);
void hashmap_clear(Hashmap *in_hashmap);
int hashmap_contains(Hashmap *in_hashmap, void *in_key);
unsigned long hashmap_count(Hashmap *in_hashmap);
void hashmap_enumerate(Hashmap *in_hashmap, void (*in_enumerator) (void *in_context,
                                                                   void *in_key, void *in_item));


/* Convenience Default Implementations */

long hashmap_hash_cstring(void *in_context, void *in_key);
int hashmap_compare_cstring(void *in_context, void *in_left, void *in_right);



#endif
