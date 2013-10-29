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
/* recognises acronynms as a single token */

#include "../../../includes/nlip.h"

#include <stdio.h>
#include <ctype.h>


static int plug_nl_acronymns_startup(void **out_context)
{
    return NL_OK;
}


static void plug_nl_acronymns_shutdown(void *in_context)
{
}


static int plug_nl_acronyms_get_next_token(void *in_context, char const *in_characters, int in_length,
                                    int *out_offset, int *out_length, NLConceptRef *out_concept,
                                    char **out_value)
{
    /* find an acronymn */
    for (int i = 0; i < in_length; i++)
    {
        if ((in_characters[i] == '.') && (i > 0) && isalpha(in_characters[i-1]) && isalpha(in_characters[i+1]))
        {
            /* find the end of it */
            i--;
            int state = 0;
            int j = i;
            for (; j < in_length; j++)
            {
                switch (state)
                {
                    case 0:
                        if (!isalpha(in_characters[j])) goto end_loop;
                        state = 1;
                        break;
                    case 1:
                        if (in_characters[j] != '.') goto end_loop;
                        state = 0;
                        break;
                }
                continue;
            end_loop:
                break;
            }
            
            /* return the match */
            *out_offset = i;
            *out_length = j - i;
            *out_concept = nl_concept("acronym");
            return NL_OK;
        }
    }
    
    /* return no match */
    return NL_ESEARCH;
}


static NLTokenizer _tokenizer_acronyms =
{
    plug_nl_acronymns_startup,
    plug_nl_acronymns_shutdown,
    plug_nl_acronyms_get_next_token,
};


void nli_bi_plug_acronym_load(void)
{
    int nl_register_tokenizer(NLTokenizer *in_tokenizer);
    nl_register_tokenizer(&_tokenizer_acronyms);
}

