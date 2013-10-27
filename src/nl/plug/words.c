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
/* recognises boundaries between words */

#include "../../../includes/nlip.h"

#include <stdio.h>
#include <ctype.h>


static int plug_nl_words_startup(void **out_context)
{
    return NL_OK;
}


static void plug_nl_words_shutdown(void *in_context)
{
}


static int plug_nl_words_get_next_end(void *in_context, char const *in_characters, int in_length,
                               int *out_end_offset, int *out_end_length, nlatts_t *in_atts)
{
    for (int i = 0; i < in_length; i++)
    {
        if (!isalpha(in_characters[i]))
        {
            /* found a word boundary; find end */
            int e = i;
            for (; e < in_length; e++)
            {
                if (isalpha(in_characters[e])) break;
            }
            
            /* return word separation */
            *out_end_offset = i;
            *out_end_length = e - i;
            return NL_OK;
        }
    }
    
    return NL_ESEARCH;
}


static NLSplitter _word_splitter =
{
    plug_nl_words_startup,
    plug_nl_words_shutdown,
    plug_nl_words_get_next_end,
};


void nli_bi_plug_ws_load(void)
{
    int nl_register_word_splitter(NLSplitter *in_splitter);
    nl_register_word_splitter(&_word_splitter);
}



