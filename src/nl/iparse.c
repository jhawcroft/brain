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
/* input natural language parsing */

#include "common.h"



void nl_input_dispose(NLInput *in_input)
{
    if (in_input->sentences) array_dispose(in_input->sentences);
    brain_free_(in_input);
}


int nl_input_parse(char const *in_utterance, NLInput **out_result)
{
    /* create a result */
    int err = NL_OK;
    *out_result = NULL;
    NLInput *input = brain_alloc_(sizeof(NLInput), 0);
    if (!input) return NL_EMEM;
    memset(input, 0, sizeof(NLInput));
    
    input->sentences = array_create(NULL, &nl_sentence_disposer_);
    if (!input->sentences) goto nl_input_parse_error;
    
    /* create a giant sentence */
    NLSentence *sentence = nl_sentence_create();
    if (!sentence) goto nl_input_parse_error;
    array_append(input->sentences, sentence);
    
    /* convert the utterance to a single large token */
    NLToken *token = nl_token_create(in_utterance, (int)strlen(in_utterance), NL_TOKFLG_TEXT, NULL, NULL);
    if (!token) goto nl_input_parse_error;
    nl_sentence_append_token(sentence, token);
    
    /* identify specific character patterns */
    for (int i = 0; i < g_nl_context.tokenizer_count; i++)
    {
        err = nl_tokenize(input, &(g_nl_context.tokenizers[i]));
        if (err) goto nl_input_parse_error;
    }
    
    /* split sentences */
    err = nl_split_sentences(input, g_nl_context.sentence_splitter);
    if (err) goto nl_input_parse_error;
    
    /* split the remaining input into words;
     lookup the words */
    err = nl_split_words(input, g_nl_context.word_splitter);
    if (err) goto nl_input_parse_error;
    
    err = nl_wordize(input, g_nl_context.wordizer);
    if (err) goto nl_input_parse_error;
    
    /* trim empty sentences */
    err = nl_trim_empty_sentences(input);
    if (err) goto nl_input_parse_error;
    
#if DEBUG == 1
    nl_input_debug(input);
#endif
    
    *out_result = input;
    return NL_OK;
    
nl_input_parse_error:
    if (input) nl_input_dispose(input);
    return err;
}




