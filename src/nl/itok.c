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
/* input nl tokenization */

#include "common.h"


/* job is to recognise special types of token, such as dates, abbreviations and acronyms using a tokenizer
starts with a single token and progressively split tokens into smaller and smaller tokens */


int nl_tokenize(NLInput *in_input, NLTokenizer *in_tokenizer)
{
    /* enumerate all sentences */
    for (int i = 0; i < array_count(in_input->sentences); i++)
    {
        NLSentence *sentence = array_item(in_input->sentences, i);
        
        for (int t = 0; t < sentence->token_count; t++)
        {
            /* only process text tokens */
            NLToken *token = sentence->tokens[t];
            if (token->flags != NL_TOKFLG_TEXT) continue;
            
            /* look for another token */
            int offset, length;
            char *value = NULL;
            NLConceptRef concept = NULL;
            int err = in_tokenizer->get_next_token(in_tokenizer->context, token->characters, token->length, &offset, &length, &concept, &value);
            if ((err != NL_OK) || (offset >= token->length))
            {
                if ((err != NL_OK) && (err != NL_ESEARCH)) return err;
                continue;
            }
            
            /* split the current token into three;
             text prior to found token, found token, text post found token */
            NLToken *pretext = NULL;
            if (offset > 0)
            {
                pretext = nl_token_create(token->characters, offset, NL_TOKFLG_TEXT, NULL, NULL);
                if (!pretext) return NL_EMEM;
            }
            
            NLToken *posttext = NULL;
            int text_length = token->length - (offset + length);
            if (text_length > 0)
            {
                posttext = nl_token_create(&(token->characters[token->length - text_length]), text_length, NL_TOKFLG_TEXT, NULL, NULL);
                if (!posttext)
                {
                    if (pretext) nl_token_dispose(pretext);
                    return NL_EMEM;
                }
            }
            
            if (offset + length > token->length)
                length = token->length - offset;
            NLToken *new_token = nl_token_create(&(token->characters[offset]), length, NL_TOKFLG_TEXT,
                                                 concept, value);
            if (!new_token)
            {
                if (pretext) nl_token_dispose(pretext);
                if (posttext) nl_token_dispose(posttext);
                return NL_EMEM;
            }
            
            /* re-jig the sentence's tokens */
            NLToken *a_token = nl_sentence_remove_token(sentence, t);
            if (!a_token)
            {
                if (pretext) nl_token_dispose(pretext);
                if (posttext) nl_token_dispose(posttext);
                if (new_token) nl_token_dispose(new_token);
                return NL_EMEM;
            }
            nl_token_dispose(a_token);
            int ip = t;
            if (pretext)
            {
                if (!nl_sentence_insert_token(sentence, ip++, pretext))
                {
                    nl_token_dispose(pretext);
                    if (posttext) nl_token_dispose(posttext);
                    nl_token_dispose(new_token);
                    return NL_EMEM;
                }
            }
            if (!nl_sentence_insert_token(sentence, ip++, new_token))
            {
                if (posttext) nl_token_dispose(posttext);
                nl_token_dispose(new_token);
                return NL_EMEM;
            }
            if (posttext)
            {
                if (!nl_sentence_insert_token(sentence, ip, posttext))
                {
                    nl_token_dispose(posttext);
                    return NL_EMEM;
                }
            }
            
            /* continue scanning where we left off */
            if (posttext)
                t++;
        }
    }
    
    return NL_OK;
}



