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
 Natural Language Meaning => Sentence Generation
 */

#include "common.h"


typedef struct nloutput_struct
{
    knpattern_t *pattern; /* purely for inspection purposes when deciding if to combine/eliminate certain sentences;
                         or how to mutate sentences to avoid sounding like a robot */
    Array *concept_seq;
    
} nloutput_t;



size_t map_references_token_(knpattern_map_t *in_map, knpattern_token_t *in_pattern_token)
{
    size_t m_arg_count = array_count(in_map->args);
    for (size_t m_arg = 0; m_arg < m_arg_count; m_arg++)
    {
        knmap_arg_t *arg = array_item(in_map->args, m_arg);
        if (arg->type == NL_MAPARG_TOKEN)
        {
            if (strcmp(arg->params.name, in_pattern_token->name) == 0)
            {
                return m_arg;
            }
        }
    }
    return ARRAY_INVALID_INDEX;
}


static knpattern_token_t* lookup_pattern_token_(knpattern_t *in_pattern, char const *in_token_name)
{
    for (knpattern_token_t *pattern_token = array_iterate(in_pattern->tokens);
         (pattern_token = array_next(in_pattern->tokens)); )
    {
        if (strcmp(pattern_token->name, in_token_name) == 0) return pattern_token;
    }
    return NULL;
}


static nloutput_t* meaning_make_output_(nlmeaning_t *in_meaning)
{
    nloutput_t *output;
    knpattern_t *pattern;
    knpattern_map_t *map;
    
    PtrSet *maps = hashmap_item(g_all_meaning_maps, in_meaning->meaning);
    if (!maps) return NULL;
    
    int m_count = (int)ptrset_count(maps);
    for (int m = 0; m < m_count; m++)
    {
        map = ptrset_item(maps, m);
        
        /* check if we have same number of arguments */
        if (array_count(map->args) != in_meaning->argument_count) continue;
        
        pattern = map->owner;
        
        /* TODO: MUST check mandatory argument types here against those supplied in meaning;
         if the argument isn't supplied (NULL) or the type isn't suitable, we skip
         this meaning map */
        
        for (int ma = 0; ma < array_count(map->args); ma++)
        {
            knmap_arg_t *mapped_arg_desc = array_item(map->args, ma);
            knconcept_t *supplied_arg = kn_concept_lookup(in_meaning->arguments[ma]);
            if (mapped_arg_desc->type == NL_MAPARG_TOKEN)
            {
                knpattern_token_t *pattern_token = lookup_pattern_token_(pattern, mapped_arg_desc->params.name);
                if (!pattern_token) goto skip_meaning_map;
                if (!kn_query_isa(supplied_arg, pattern_token->concept)) goto skip_meaning_map;
            }
            else if (mapped_arg_desc->type == NL_MAPARG_CONCEPT)
            {
                if (!kn_query_isa(supplied_arg, kn_concept_lookup(mapped_arg_desc->params.name)))
                    goto skip_meaning_map;
            }
        }
        
        
        /* found a matching meaning map */
        goto matched_meaning_map;
        
    skip_meaning_map:
        continue;
    }
    
    /* no matching meaning map;
     I guess eventually we should have a way to find similar meanings
     so that we can try to 'speak',
     right now I'm not sure what will happen */
    return NULL;
    
matched_meaning_map:
    /* got pattern (NLPattern*) and meaning map (NLMeaningMap*) */
    
    output = brain_alloc_(sizeof(nloutput_t), 0);
    output->pattern = pattern;
    output->concept_seq = array_create(NULL, NULL);

    for (knpattern_token_t *pattern_token = array_iterate(pattern->tokens);
         (pattern_token = array_next(pattern->tokens)); )
    {
        size_t arg_index = map_references_token_(map, pattern_token);
        if (arg_index == ARRAY_INVALID_INDEX)
            array_append(output->concept_seq, pattern_token->concept);
        else
            array_append(output->concept_seq, kn_concept_lookup(in_meaning->arguments[arg_index]));
    }
    
    return output;
}


char const* concept_make_representation_(knconcept_t *in_concept)
{
    for (knrep_t *rep = array_iterate(in_concept->reps);
         (rep = array_next(in_concept->reps)); )
    {
        return rep->text;
    }
    return NULL;
}


int nl_meanings_to_output(nlmeaning_t *in_meanings[], int in_count, char **out_output, int in_hints)
{
    Sbuff *result = sbuff_create(SBUFF_DEFAULT_CAPACITY);
    
    for (int m = 0; m < in_count; m++)
    {
        nlmeaning_t *meaning = in_meanings[m];
        nloutput_t *output = meaning_make_output_(meaning);
        
        size_t concept_count = array_count(output->concept_seq);
        for (size_t c = 0; c < concept_count; c++)
        {
            knconcept_t *concept = array_item(output->concept_seq, c);
            sbuff_append_cstring(result, concept_make_representation_(concept), SBUFF_AUTO_LENGTH);
            if (c + 1 < concept_count)
                sbuff_append_cstring(result, " ", 1);
            else
            {
                sbuff_append_cstring(result, ". ", 1);
            }
        }
    }
    
    *out_output = sbuff_cstring_disposing(result);
    
    return NL_OK;
}





