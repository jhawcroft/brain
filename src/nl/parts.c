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
/* natural language part utilities */

#include "common.h"


#define SENTENCE_ATTR_BUCKETS 2


/* Sentences */

NLSentence* nl_sentence_create(void)
{
    NLSentence *sentence = brain_alloc_(sizeof(NLSentence), 0);
    if (!sentence) return NULL;
    memset(sentence, 0, sizeof(NLSentence));
    sentence->attributes = hashmap_create(SENTENCE_ATTR_BUCKETS,
                                          NULL,
                                          &hashmap_hash_cstring,
                                          &hashmap_compare_cstring,
                                          &cont_dispose_key_,
                                          &cont_dispose_key_value_);
    if (!sentence->attributes)
    {
        free(sentence);
        return NULL;
    }
    return sentence;
}


void nl_sentence_dispose(NLSentence *in_sentence)
{
    for (int t = 0; t < in_sentence->token_count; t++)
        nl_token_dispose(in_sentence->tokens[t]);
    if (in_sentence->tokens)
        brain_free_(in_sentence->tokens);
    hashmap_dispose(in_sentence->attributes);
    brain_free_(in_sentence);
}


void nl_sentence_disposer_(void *in_context, void *in_item)
{
    nl_sentence_dispose(in_item);
}


NLToken* nl_sentence_insert_token(NLSentence *in_sentence, int in_at_index, NLToken *in_token)
{
    NLToken **new_tokens = brain_realloc_(in_sentence->tokens,
                                        sizeof(NLToken*) * (in_sentence->token_count + 1),
                                          0);
    if (!new_tokens) return NULL;
    in_sentence->tokens = new_tokens;
    
    if (! memmove(&(new_tokens[in_at_index+1]),
                  &(new_tokens[in_at_index]),
                  sizeof(NLToken*) * (in_sentence->token_count - in_at_index)) )
        return NULL;
    
    new_tokens[in_at_index] = in_token;
    in_sentence->token_count++;
    
    return in_token;
}


NLToken* nl_sentence_append_token(NLSentence *in_sentence, NLToken *in_token)
{
    NLToken **new_tokens = brain_realloc_(in_sentence->tokens,
                                          sizeof(NLToken*) * (in_sentence->token_count + 1),
                                          0);
    if (!new_tokens) return NULL;
    in_sentence->tokens = new_tokens;
    new_tokens[in_sentence->token_count++] = in_token;
    return in_token;
}


NLToken* nl_sentence_remove_token(NLSentence *in_sentence, int in_index)
{
    NLToken *token = in_sentence->tokens[in_index];
    if (! memmove(&(in_sentence->tokens[in_index]),
                  &(in_sentence->tokens[in_index+1]),
                  sizeof(NLToken*) * (in_sentence->token_count - in_index - 1)) )
        return NULL;
    in_sentence->token_count--;
    return token;
}


/* Tokens */

NLToken* nl_token_create(char const *in_string, int in_length, int in_flags,
                         NLConceptRef in_concept, char const *in_value)
{
    NLToken *token = brain_alloc_(sizeof(NLToken), 0);
    if (!token) return NULL;
    memset(token, 0, sizeof(NLToken));
    
    token->characters = strndup(in_string, in_length); /* TODO: use our own; so we can use our allocator */
    if (!token->characters) goto nl_token_create_error;
    token->length = in_length;
    
    token->concepts = ptrset_create(NULL, NULL);
    if (!token->concepts) goto nl_token_create_error;
    if (in_concept != NULL)
    {
        if (!ptrset_add(token->concepts, in_concept)) goto nl_token_create_error;
    }
    
    token->value.type = NL_VAL_STRING;
    token->value.data.string = strdup(in_value);
    if (!token->value.data.string) goto nl_token_create_error;
    
    token->flags = in_flags;
    
    return token;
    
nl_token_create_error:
    nl_token_dispose(token);
    return NULL;
}


void nl_token_dispose(NLToken *in_token)
{
    if (in_token->characters) brain_free_(in_token->characters);
    if (in_token->concepts) ptrset_dispose(in_token->concepts);
    if (in_token->value.data.string) brain_free_(in_token->value.data.string);
    if (in_token) brain_free_(in_token);
    free(in_token);
}






