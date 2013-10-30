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
/* Knowledge network bootstrap */

#include "common.h"


void kn_bootstrap_(void)
{
    /* load knowledge */
    kn_concept_define("language");
    
    knconcept_t *concept1, *concept2;
    
    
    kn_concept_define("agent");
    
    
    concept1 = kn_concept_define("English");
    
    concept2 = kn_concept_define("French");
    kn_relate_isa(concept2, kn_concept_lookup("language"));
    kn_rep_add(concept2, "French", concept1);
    kn_rep_add(concept2, "Français", concept2);
    
    concept1 = kn_concept_define("English");
    kn_relate_isa(concept1, kn_concept_lookup("language"));
    kn_rep_add(concept1, "English", concept1);
    kn_rep_add(concept1, "Anglais", concept2);
    
    
    knconcept_t *lang = kn_concept_lookup("English");
    
    
    concept1 = kn_concept_define("agent-writer");
    kn_relate_isa(concept1, kn_concept_lookup("agent"));
    kn_rep_add(concept1, "I", lang);
    kn_rep_add(concept1, "myself", lang);
    kn_rep_add(concept1, "me", lang);
    
    concept1 = kn_concept_define("agent-reader");
    kn_relate_isa(concept1, kn_concept_lookup("agent"));
    kn_rep_add(concept1, "you", lang);
    
    concept1 = kn_concept_define("to-like");
    kn_rep_add(concept1, "like", lang);
    kn_rep_add(concept1, "love", lang);
    kn_rep_add(concept1, "enjoy", lang);
    
    concept1 = kn_concept_define("how-much-like");
    kn_rep_add(concept1, "really", lang);
    kn_rep_add(concept1, "often", lang);
    kn_rep_add(concept1, "sometimes", lang);
    kn_rep_add(concept1, "kinda", lang);
    
    
    concept1 = kn_concept_define("food");
    
    concept2 = kn_concept_define("meat");
    kn_relate_isa(concept2, concept1);
    kn_rep_add(concept2, "meat", lang);
    
    concept1 = kn_concept_define("chicken");
    kn_relate_isa(concept1, concept2);
    kn_rep_add(concept1, "chicken", lang);
    
    concept1 = kn_concept_define("beef");
    kn_relate_isa(concept1, concept2);
    kn_rep_add(concept1, "beef", lang);
    
    
    concept1 = kn_concept_define("salutation");
    
    concept2 = kn_concept_define("greeting");
    kn_relate_isa(concept2, concept1);
    kn_rep_add(concept2, "hi", lang);
    kn_rep_add(concept2, "g'day", lang);
    kn_rep_add(concept2, "hello", lang);
    kn_rep_add(concept2, "howdy", lang);
    
    
    /* load natural language patterns */
    knpattern_t *pattern1;
    knpattern_map_t *map1;
    
    pattern1 = kn_pattern_define(KN_CAT_GENERAL, lang); /* temporarily wrong category for testing */
    kn_token_append(pattern1, "greeting-used", kn_concept_lookup("greeting"), false);
    map1 = kn_map_append(pattern1, "hello");
    kn_arg_add_token_val(map1, "greeting-used");
    
    pattern1 = kn_pattern_define(KN_CAT_GENERAL, lang);
    kn_token_append(pattern1, "who", kn_concept_lookup("agent-writer"), false);
    kn_token_append(pattern1, "adjective", kn_concept_lookup("how-much-like"), true);
    kn_token_append(pattern1, "verb", kn_concept_lookup("to-like"), false);
    kn_token_append(pattern1, "what", kn_concept_lookup("meat"), false);
    map1 = kn_map_append(pattern1, "like-something");
    kn_arg_add_token_val(map1, "verb");
    kn_arg_add_token_val(map1, "adjective");
    kn_arg_add_token_val(map1, "who");
    kn_arg_add_token_val(map1, "what");
}





