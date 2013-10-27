//
//  set.h
//  AI-NLPartA-IdeaA
//
//  Created by Joshua Hawcroft on 12/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#ifndef AI_NLPartA_IdeaA_set_h
#define AI_NLPartA_IdeaA_set_h

typedef struct Set_ Set;


#define SET_COMPILE_SELF_TESTS 1


/* now we should implement intersection, union and subtraction/difference;
 we could well have uses for all of these in reducing the patterns to match eventually */


Set* set_create(void);
void set_dispose(Set *in_set);

void set_add(Set *in_set, void *in_item);
void set_remove(Set *in_set, void *in_item);

int set_contains(Set *in_set, void *in_item);
int set_is_empty(Set *in_set);

unsigned long set_count(Set *in_set);
void* set_item(Set *in_set, unsigned long in_index);

void set_union(Set *in_dest, Set *in_src);
void set_intersection(Set *in_dest, Set *in_src);
void set_difference(Set *in_dest, Set *in_src);


#if SET_COMPILE_SELF_TESTS == 1
void set_self_test(void);
#endif


#endif
