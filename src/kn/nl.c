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

#include "common.h"


PtrSet* kn_text_to_concept_set(char const *in_text)
{
    PtrSet *result = ptrset_create(NULL, NULL);
    PtrSet *reps = hashmap_item(g_all_representations, (void*)in_text);
    if (!reps) return result;
    for (int r = 0; r < ptrset_count(reps); r++)
    {
        knrep_t *rep = ptrset_item(reps, r);
        ptrset_add(result, rep->owner);
    }
    return result;
}


/*




Set* kb_token_text_to_pattern_set(char const *in_text)
{
    Set *pattern_set = set_create();
    
    Set *representations = hashmap_item(g_all_representations, (void*)in_text);
    for (int r = 0; r < set_count(representations); r++)
    {
        NLRepresentation *representation = set_item(representations, r);
        Concept *concept = representation->referencing_concept;
        printf("Concept: %s\n", concept->programming_name);
        
        set_union(pattern_set, concept->referencing_patterns);
    }
    
    return pattern_set;
}



Array* kb_meaning_maps_for_meaning(Concept *in_concept)
{
    return hashmap_item(g_all_meaning_maps, (void*)in_concept->programming_name);
    // should be using the concept pointer itself - or an ID - not the programming name for this kind of thing
    // check elsewhere too  ***
}*/




