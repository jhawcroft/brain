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
/* input plugin manager */

#include "common.h"



int nl_register_tokenizer(NLTokenizer *in_tokenizer)
{
    NLTokenizer *new_tokenizers
    = brain_realloc_(g_nl_context.tokenizers,
                     sizeof(NLTokenizer) * (g_nl_context.tokenizer_count + 1),
                     0);
    if (!new_tokenizers) return NL_EMEM;
    g_nl_context.tokenizers = new_tokenizers;
    
    new_tokenizers[g_nl_context.tokenizer_count] = *in_tokenizer;
    NLTokenizer *tokenizer = &(new_tokenizers[g_nl_context.tokenizer_count++]);
    
    tokenizer->context = NULL;
    int err = tokenizer->startup(&(tokenizer->context));
    if (err)
    {
        g_nl_context.tokenizer_count--;
        return err;
    }
    
    return NL_OK;
}


int nl_register_sentence_splitter(NLSplitter *in_splitter)
{
    g_nl_context.sentence_splitter = in_splitter;
    in_splitter->startup(&(in_splitter->context));
    
    return NL_OK;
}


int nl_register_word_splitter(NLSplitter *in_splitter)
{
    g_nl_context.word_splitter = in_splitter;
    in_splitter->startup(&(in_splitter->context));
    
    return NL_OK;
}


int nl_register_wordizer(NLWordizer *in_wordizer)
{
    g_nl_context.wordizer = in_wordizer;
    in_wordizer->startup(&(in_wordizer->context));
    
    return NL_OK;
}


void nl_add_attribute(nlatts_t *in_atts, char const *in_att_name, char const *in_att_value)
{
    hashmap_add((Hashmap*)in_atts, brain_strdup(in_att_name), brain_strdup(in_att_value));
}




