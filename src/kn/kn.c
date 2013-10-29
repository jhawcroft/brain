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


Hashmap *g_all_representations; /* of Set, of knrep_t,
                                 provides a quick way to lookup concepts by 
                                 natrual language representation */

Hashmap *g_all_concepts; /* concepts stored by programming name 
                          (knconcept_t) */


/* patterns for matching;
 TODO: eventually we will need to organise these to facilite removal of many
 patterns that don't match a single combination of input tokens */
Array *g_patterns_salutations;
Array *g_patterns_general;


Hashmap *g_all_meaning_maps; /* of Set, of knpattern_map_t,
                              meaning maps organised by meaning string
                              for quick access during natrual language 
                              generation */


int kn_startup(void)
{
    g_all_representations = hashmap_create(4000, NULL, &hashmap_utf8_hash_, &hashmap_utf8_compare_, NULL, NULL);
    g_all_concepts = hashmap_create(4000, NULL, &hashmap_utf8_hash_, &hashmap_utf8_compare_, NULL, NULL);
    
    g_patterns_salutations = array_create(NULL, NULL); /* TODO: put a deallocator here  ?? or manage directly? ****/
    g_patterns_general = array_create(NULL, NULL); /* AS ABOVE / TODO */
    g_all_meaning_maps = hashmap_create(200, NULL, &hashmap_utf8_hash_, &hashmap_utf8_compare_, NULL, NULL); /* AS ABOVE / TODO */
    
    return KN_OK;
}






