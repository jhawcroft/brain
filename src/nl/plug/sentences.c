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
/* recognises the divisons between sentences */

#include "../../../includes/nlip.h"

#include <stdio.h>


static int plug_nl_sentences_startup(void **out_context)
{
    return NL_OK;
}


static void plug_nl_sentences_shutdown(void *in_context)
{
}


static int plug_nl_sentences_get_next_end(void *in_context, char const *in_characters, int in_length, int *out_end_offset, int *out_end_length)
{
    /* find end of current sentence */
    for (int i = 0; i < in_length; i++)
    {
        if ((in_characters[i] == '.') || (in_characters[i] == '!') || (in_characters[i] == '?'))
        {
            /* return sentence attributes and position of punctuation to remove */
            *out_end_offset = i;
            *out_end_length = 1;
            return NL_OK;
        }
    }
    
    return NL_ESEARCH;
}


static NLSplitter _sentence_splitter =
{
    plug_nl_sentences_startup,
    plug_nl_sentences_shutdown,
    plug_nl_sentences_get_next_end,
};


void nli_bi_plug_ss_load(void)
{
    int nl_register_sentence_splitter(NLSplitter *in_splitter);
    nl_register_sentence_splitter(&_sentence_splitter);
}



