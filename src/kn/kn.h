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
 Program interface for the knowledge network
 */

#ifndef BRAIN_KN_H
#define BRAIN_KN_H

#include <stdbool.h>
#include "../cont/ptrset.h"


typedef struct knconcept_struct knconcept_t;


typedef void* ConceptRef;
typedef void* PatternRef;


enum
{
    KN_OK = 0,
    KN_EMEM,
};


int kn_startup(void);


/* TODO: provide functions to explore the concepts, representations and relationships
 for management;
 do only once a language has been defined and it's more clear precisely what functions will be needed. */


knconcept_t* kn_concept_define(char const *in_name);
void kn_concept_forget(knconcept_t *in_concept);

int kn_rep_add(knconcept_t *in_concept, char const *in_rep, knconcept_t *in_lang);

void kn_relate_isa(knconcept_t *in_sub_concept, knconcept_t *in_super_concept);

int kn_query_isa(knconcept_t *in_sub_concept, knconcept_t *in_super_concept);



knconcept_t* kn_concept_lookup(char const *in_name);
PtrSet* kn_text_to_concept_set(char const *in_text);

char const* kn_concept_name(knconcept_t *in_concept);



typedef struct knpattern_struct knpattern_t;
typedef struct knpattern_map_struct knpattern_map_t;

enum
{
    KN_CAT_SALUTATION,
    KN_CAT_GENERAL,
    
};

knpattern_t* kn_pattern_define(int in_category, knconcept_t *in_lang);
void kn_token_append(knpattern_t *in_pattern, char const *in_name, knconcept_t *in_concept, bool in_optional);

/* TODO: the most complicated patterns have to end up first;
 which implies that we will need to be able to create a pattern that isn't registered, 
 in stages, and then register it/'activate it' once it's done */
/* only inactive patterns would be modifyable */

knpattern_map_t* kn_map_append(knpattern_t *in_pattern, char const *in_meaning);
void kn_arg_add_token_val(knpattern_map_t *in_map, char const *in_token_name);
void kn_arg_add_concept(knpattern_map_t *in_map, knconcept_t *in_concept);





#endif
