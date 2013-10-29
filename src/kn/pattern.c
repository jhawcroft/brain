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


static void pattok_dispose_cont_(void *in_context, knpattern_token_t *in_token)
{
    
}


static void patmap_dispose_cont_(void *in_context, knpattern_map_t *in_map)
{
    
}


knpattern_t* kn_pattern_define(int in_category, knconcept_t *in_lang)
{
    knpattern_t *pattern = brain_alloc_(sizeof(knpattern_t), 0);
    memset(pattern, 0, sizeof(knpattern_t));
    
    pattern->tokens = array_create(NULL, (ArrayCBDisposeItem)pattok_dispose_cont_);
    pattern->lang = in_lang;
    pattern->maps = array_create(NULL, (ArrayCBDisposeItem)patmap_dispose_cont_);
    
    if (in_category == KN_CAT_SALUTATION)
        array_append(g_patterns_salutations, pattern);
    else
        array_append(g_patterns_general, pattern);
    
    return pattern;
}


void kn_token_append(knpattern_t *in_pattern, char const *in_name, knconcept_t *in_concept, bool in_optional)
{
    knpattern_token_t *token = brain_alloc_(sizeof(knpattern_token_t), 0);
    memset(token, 0, sizeof(knpattern_token_t));
    
    token->name = brain_strdup(in_name);
    token->concept = in_concept;
    token->optional = in_optional;
    
    array_append(in_pattern->tokens, token);
}


static void maparg_dispose_cont_(void *in_context, knmap_arg_t *in_arg)
{
    
}


knpattern_map_t* kn_map_append(knpattern_t *in_pattern, char const *in_meaning)
{
    knpattern_map_t *map = brain_alloc_(sizeof(knpattern_map_t), 0);
    memset(map, 0, sizeof(knpattern_map_t));
    
    map->meaning = brain_strdup(in_meaning);
    map->owner = in_pattern;
    map->args = array_create(NULL, (ArrayCBDisposeItem)maparg_dispose_cont_);
    
    array_append(in_pattern->maps, map);
    
    PtrSet *maps = hashmap_item(g_all_meaning_maps, map->meaning);
    if (!maps)
    {
        maps = ptrset_create(NULL, NULL);
        hashmap_add(g_all_meaning_maps, brain_strdup(map->meaning), maps);
    }
    ptrset_add(maps, map);
    
    return map;
}


static knmap_arg_t* create_arg(knpattern_map_t *in_map)
{
    knmap_arg_t *arg = brain_alloc_(sizeof(knmap_arg_t), 0);
    memset(arg, 0, sizeof(knmap_arg_t));
    array_append(in_map->args, arg);
    return arg;
}


void kn_arg_add_token_val(knpattern_map_t *in_map, char const *in_token_name)
{
    knmap_arg_t *arg = create_arg(in_map);
    arg->type = NL_MAPARG_TOKEN;
    arg->params.name = brain_strdup(in_token_name);
}


void kn_arg_add_concept(knpattern_map_t *in_map, knconcept_t *in_concept)
{
    knmap_arg_t *arg = create_arg(in_map);
    arg->type = NL_MAPARG_CONCEPT;
    arg->params.name = brain_strdup(in_concept->name);
}









