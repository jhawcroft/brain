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
 Knowledge network internal structures
 */

#ifndef BRAIN_KN_STRUCTS_H
#define BRAIN_KN_STRUCTS_H


#include "common.h"




typedef struct knrep_struct knrep_t;


struct knrep_struct
{
    char *text;
    knconcept_t *lang;
    knconcept_t *owner;
    
};


struct knconcept_struct
{
    char *name;
    
    Array *reps; /* array of knrep_t */
    
    /* other Concepts */
    Array *supers;
    Array *subs;
};






typedef struct
{
    enum
    {
        NL_MAPARG_UNDEFINED = 0,
        NL_MAPARG_TOKEN,
        NL_MAPARG_CONCEPT,
        
    } type;
    union
    {
        char *name;
        
    } params;
} knmap_arg_t;


struct knpattern_map_struct
{
    char *meaning;
    Array *args; /* array of nlmaparg_t */
    
    knpattern_t *owner;
    
};


typedef struct
{
    char *name;
    knconcept_t *concept;
    bool optional;
    
} knpattern_token_t;


struct knpattern_struct
{
    Array *tokens; /* array of NLPatternToken */
    knconcept_t *lang;
    Array *maps; /* array of NLPatternMap */
};



extern Hashmap *g_all_representations;
extern Hashmap *g_all_concepts;
extern Array *g_patterns_salutations;
extern Array *g_patterns_general;
extern Hashmap *g_all_meaning_maps;



#endif
