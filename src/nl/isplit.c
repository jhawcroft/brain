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
/* input nl splitting */

#include "common.h"


int nl_trim_empty_sentences(NLInput *in_input)
{
    /* enumerate all sentences */
    for (int i = 0; i < array_count(in_input->sentences); i++)
    {
        NLSentence *sentence = array_item(in_input->sentences, i);
        if (sentence->token_count == 0)
        {
            if (array_remove(in_input->sentences, i, NULL)) return NL_EMEM;
            i--;
        }
    }
    return NL_OK;
}


int nl_split_sentences(NLInput *in_input, NLSplitter *in_splitter)
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
            
            /* create a new sentence */
            NLSentence *another_sentence = nl_sentence_create();
            if (!another_sentence) return NL_EMEM;
            
            /* look for another split delimiter */
            int end_offset, end_length;
            int err = in_splitter->get_next_end_point(in_splitter->context,
                                                      token->characters, token->length,
                                                      &end_offset, &end_length,
                                                      (nlatts_t*)another_sentence->attributes);
            if ((err != NL_OK) || (end_offset > token->length))
            {
                nl_sentence_dispose(another_sentence);
                if ((err != NL_OK) && (err != NL_ESEARCH)) return err;
                continue;
            }
            
            /* create new pretext and posttext tokens */
            NLToken *pretext = NULL;
            if (end_offset > 0)
            {
                pretext = nl_token_create(token->characters, end_offset, NL_TOKFLG_TEXT, NULL, NULL);
                if (!pretext)
                {
                    nl_sentence_dispose(another_sentence);
                    return NL_EMEM;
                }
            }
            
            NLToken *posttext = NULL;
            int text_length = token->length - (end_offset + end_length);
            if (text_length > 0)
            {
                posttext = nl_token_create(&(token->characters[token->length - text_length]), text_length, NL_TOKFLG_TEXT, NULL, NULL);
                if (!posttext)
                {
                    nl_sentence_dispose(another_sentence);
                    return NL_EMEM;
                }
            }
            
            /* append new sentence */
            if (!array_append(in_input->sentences, another_sentence))
            {
                nl_sentence_dispose(another_sentence);
                return NL_EMEM;
            }
            
            /* append the posttext to the new sentence */
            if (posttext)
            {
                if (!nl_sentence_append_token(another_sentence, posttext))
                {
                    nl_token_dispose(posttext);
                    if (pretext) nl_token_dispose(pretext);
                    return NL_EMEM;
                }
            }
            
            /* move all tokens beyond this one to the new sentence */
            for (int at = t + 1; at < sentence->token_count; at++)
            {
                if (!nl_sentence_append_token(another_sentence, sentence->tokens[at]))
                {
                    if (pretext) nl_token_dispose(pretext);
                    return NL_EMEM;
                }
                sentence->tokens[at] = NULL;
            }
            if (t + 1 < sentence->token_count)
                sentence->token_count -= (sentence->token_count - (t+1));
            
            /* replace the token with the pretext token */
            NLToken *a_token = nl_sentence_remove_token(sentence, t);
            if (!a_token)
            {
                if (pretext) nl_token_dispose(pretext);
                return NL_EMEM;
            }
            nl_token_dispose(a_token);
            if (pretext)
            {
                if (!nl_sentence_append_token(sentence, pretext))
                {
                    nl_token_dispose(pretext);
                    return NL_EMEM;
                }
            }
        }
    }
    
    return NL_OK;
}


int nl_split_words(NLInput *in_input, NLSplitter *in_splitter)
{
    /* enumerate all sentences */
    for (int i = 0; i < array_count(in_input->sentences); i++)
    {
        NLSentence *sentence = array_item(in_input->sentences, i);
        
        for (int t = 0; t < sentence->token_count; t++)
        {
            /* only process text tokens */
            if (t < 0) break;
            NLToken *token = sentence->tokens[t];
            if (token->flags != NL_TOKFLG_TEXT) continue;
            
            /* look for another split delimiter */
            int end_offset, end_length;
            int err = in_splitter->get_next_end_point(in_splitter->context, token->characters, token->length,
                                                      &end_offset, &end_length, NULL);
            if ((err != NL_OK) || (end_offset > token->length))
            {
                if ((err != NL_OK) && (err != NL_ESEARCH)) return err;
                continue;
            }
            
            /* create new pretext and posttext tokens */
            NLToken *pretext = NULL;
            if (end_offset > 0)
            {
                pretext = nl_token_create(token->characters, end_offset, NL_TOKFLG_TEXT, NULL, NULL);
                if (!pretext) return NL_EMEM;
            }
            
            NLToken *posttext = NULL;
            int text_length = token->length - (end_offset + end_length);
            if (text_length > 0)
            {
                posttext = nl_token_create(&(token->characters[token->length - text_length]), text_length, NL_TOKFLG_TEXT, NULL, NULL);
                if (!posttext)
                {
                    if (pretext) nl_token_dispose(pretext);
                    return NL_EMEM;
                }
            }
            
            /* re-jig the tokens */
            NLToken *a_token = nl_sentence_remove_token(sentence, t);
            if (!a_token)
            {
                if (posttext) nl_token_dispose(posttext);
                if (pretext) nl_token_dispose(pretext);
                return NL_EMEM;
            }
            nl_token_dispose(a_token);
            t--;
            if (pretext)
            {
                if (!nl_sentence_insert_token(sentence, t + 1, pretext))
                {
                    if (posttext) nl_token_dispose(posttext);
                    nl_token_dispose(pretext);
                    return NL_EMEM;
                }
                t++;
            }
            if (posttext)
            {
                if (!nl_sentence_insert_token(sentence, t + 1, posttext))
                {
                    nl_token_dispose(posttext);
                    return NL_EMEM;
                }
            }
        }
    }
    
    return NL_OK;
}



