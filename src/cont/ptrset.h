/*
 PtrSet
 v. 1.1
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 Implements a set of unique pointers.
 
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

#ifndef AI_NLPartA_IdeaA_set_h
#define AI_NLPartA_IdeaA_set_h

typedef struct PtrSet_ PtrSet;


#define PTRSET_COMPILE_SELF_TESTS 1


typedef void (*PtrSetCBDisposeItem) (void *in_context, void *in_item);


PtrSet* ptrset_create(void *in_context, PtrSetCBDisposeItem in_dis_item);
void ptrset_dispose(PtrSet *in_set);

void* ptrset_add(PtrSet *in_set, void *in_item);
void ptrset_remove(PtrSet *in_set, void *in_item);

int ptrset_contains(PtrSet *in_set, void *in_item);
int ptrset_is_empty(PtrSet *in_set);

unsigned long ptrset_count(PtrSet *in_set);
void* ptrset_item(PtrSet *in_set, unsigned long in_index);

void ptrset_union(PtrSet *in_dest, PtrSet *in_src);
void ptrset_intersection(PtrSet *in_dest, PtrSet *in_src);
void ptrset_difference(PtrSet *in_dest, PtrSet *in_src);


#if PTRSET_COMPILE_SELF_TESTS == 1
void ptrset_self_test(void);
#endif


#endif

