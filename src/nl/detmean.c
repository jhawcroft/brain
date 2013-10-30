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
 Natural Language Sentence => Meaning Understanding
 */

#include "common.h"


/*
 In future, could be optimised by progressively removing patterns
 from the search list as successive patterns fail to match.
 
 Initially based on the first concept match only, and then gradually on greater combinations.
 */

static bool match_sentence_to_patterns_(NLSentence *in_sentence, Array *in_patterns,
                                        knpattern_t **out_pattern, knconcept_t *out_matching_concepts[])
{
    int sentence_indicies[MAX_PATTERN_TOKENS];
    knpattern_t *pattern = NULL;
    int token_count = in_sentence->token_count;
    
    /* consider each pattern */
    size_t pattern_count = array_count(in_patterns);
    for (size_t p = 0; p < pattern_count; p++)
    {
        pattern = array_item(in_patterns, p);
        
        int token_index = 0;
        
        int t_count = (int)array_count(pattern->tokens);
        for (int t = 0; t < t_count; t++)
        {
            knpattern_token_t *pattern_token = array_item(pattern->tokens, t);
            
            /* consider only mandatory pattern tokens initially */
            if (pattern_token->optional)
            {
                out_matching_concepts[t] = NULL;
                sentence_indicies[t] = 9999;
                continue;
            }
            
            for (;;)
            {
                NLToken *current_token = in_sentence->tokens[token_index++];
                
                for (int c = 0; c < ptrset_count(current_token->concepts); c++)
                {
                    if (kn_query_isa(ptrset_item(current_token->concepts, c), pattern_token->concept))
                    {
                        out_matching_concepts[t] = ptrset_item(current_token->concepts, c);
                        sentence_indicies[t] = token_index-1;
                        goto matched_query;
                    }
                }
                if (token_index >= token_count) goto pattern_failed;
            }
            
        matched_query:
            //matched_query_token_indicies[q] = token_index - 1;
            
            if (t + 1 == t_count)
                goto matched_pattern;
        }
        
    pattern_failed:
        continue;
    }
    return false;
    
matched_pattern:
    *out_pattern = pattern;
    
    /* now consider optional pattern tokens;
     currently implements a left->right search direction */
    int last_match_index = 0;
    int t_count = (int)array_count(pattern->tokens);
    for (int t = 0; t < t_count; t++)
    {
        knpattern_token_t *pattern_token = array_item(pattern->tokens, t);
        if (pattern_token->optional)
        {
            int sentence_from = sentence_indicies[last_match_index] + 1;
            int sentence_to = in_sentence->token_count - 1;
            for (int i = t + 1; i < t_count; i++)
            {
                knpattern_token_t *ahead_token = array_item(pattern->tokens, i);
                if (!ahead_token->optional)
                {
                    sentence_to = sentence_indicies[i] - 1;
                    break;
                }
            }
            
            if ((sentence_from <= sentence_to) && (sentence_from < in_sentence->token_count))
            {
                for (int i = sentence_from; i <= sentence_to; i++)
                {
                    NLToken *current_token = in_sentence->tokens[i];
                    
                    for (int c = 0; c < ptrset_count(current_token->concepts); c++)
                    {
                        if (kn_query_isa(ptrset_item(current_token->concepts, c), pattern_token->concept))
                        {
                            out_matching_concepts[t] = ptrset_item(current_token->concepts, c);
                            sentence_indicies[t] = i;
                            last_match_index = t;
                            goto matched_optional;
                        }
                    }
                }
            }
            
        }
        else
            last_match_index = t;
        
    matched_optional:
        continue;
    }
    
    return true;
}


static size_t named_token_index(knpattern_t *in_pattern, char const *in_name)
{
    for (int t = 0; t < array_count(in_pattern->tokens); t++)
    {
        knpattern_token_t *pattern_token = array_item(in_pattern->tokens, t);
        if (strcmp(pattern_token->name, in_name) == 0) return t;
    }
    return ARRAY_INVALID_INDEX;
}


static nlmeaning_t* map_tokens_(knpattern_t *in_pattern, knpattern_map_t *in_map, knconcept_t **in_matched_concepts)
{
    nlmeaning_t *result = brain_alloc_(sizeof(nlmeaning_t), 0);
    memset(result, 0, sizeof(nlmeaning_t));
    
    result->meaning = brain_strdup(in_map->meaning);
    result->argument_count = (int)array_count(in_map->args);
    result->arguments = brain_alloc_(sizeof(char*) * result->argument_count, 0);
    
    for (int a = 0; a < result->argument_count; a++)
    {
        result->arguments[a] = NULL;
        knmap_arg_t *map_arg = array_item(in_map->args, a);
        if (map_arg->type == NL_MAPARG_TOKEN)
        {
            size_t token_index = named_token_index(in_pattern, map_arg->params.name);
            if (token_index != ARRAY_INVALID_INDEX)
            {
                if (in_matched_concepts[token_index])
                    result->arguments[a] = brain_strdup(in_matched_concepts[token_index]->name);
                else
                    result->arguments[a] = NULL;
            }
        }
        else if (map_arg->type == NL_MAPARG_CONCEPT)
        {
            result->arguments[a] = brain_strdup(map_arg->params.name);
        }
    }
    
    return result;
}


Array* nl_sentence_to_meanings(NLSentence *in_sentence)
{
    knconcept_t *matched_concepts[MAX_PATTERN_TOKENS];
    knpattern_t *pattern;
    Array *result = array_create(NULL, NULL);
    
    /* TODO: may want to organise this to consider all patterns;
     such that the longest match, ie. greatest number of required + matching optional concepts
     is picked; the most complicated should be the most accurate */
    
    if (match_sentence_to_patterns_(in_sentence, g_patterns_general, &pattern, matched_concepts))
    {
        for (int m = 0; m < array_count(pattern->maps); m++)
        {
            array_append(result, map_tokens_(pattern, array_item(pattern->maps, m), matched_concepts));
        }
    }
    
    if (array_count(result) == 0)
    {
        array_dispose(result);
        return NULL;
    }
    
    return result;
}


nlmeaning_t* nl_cant_understand(NLSentence *in_sentence)
{
    nlmeaning_t *result = brain_alloc_(sizeof(nlmeaning_t), 0);
    memset(result, 0, sizeof(nlmeaning_t));
    result->meaning = brain_strdup("cant-understand");
    return result;
}


int nl_input_to_meanings(char const *in_utterance, nlmeaning_t **out_meanings[], int *out_count)
{
    NLInput *input;
    int err = nl_input_parse(in_utterance, &input);
    if (err) return err;
    
    Array *result = array_create(NULL, NULL);
    
    for (int s = 0; s < array_count(input->sentences); s++)
    {
        NLSentence *sentence = array_item(input->sentences, s);
        
        Array *meanings = nl_sentence_to_meanings(sentence);
        if (meanings)
        {
            array_union(result, meanings);
            array_dispose(meanings);
        }
        else
            array_append(result, nl_cant_understand(sentence));
    }
    
    *out_meanings = brain_alloc_(sizeof(nlmeaning_t*) * array_count(result), 0);
    *out_count = (int)array_count(result);
    for (int m = 0; m < *out_count; m++)
    {
        (*out_meanings)[m] = array_item(result, m);
    }
    
    array_dispose(result);
    return NL_OK;
}







