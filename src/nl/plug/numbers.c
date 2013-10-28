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
/* recognises numbers as a single token */

#include "../../../includes/nlip.h"

#include <stdio.h>
#include <ctype.h>


static int plug_nl_numbers_startup(void **out_context)
{
    return NL_OK;
}


static void plug_nl_numbers_shutdown(void *in_context)
{
}


static int plug_nl_numbers_get_next_token(void *in_context, char const *in_characters, int in_length,
                                    int *out_offset, int *out_length, NLConceptRef *out_concept,
                                    char **out_value)
{
    /* find a number */
    for (int i = 0; i < in_length; i++)
    {
        if ((isdigit(in_characters[i])) ||
            ( (i + 1 < in_length) && (in_characters[i] == '.') && (isdigit(in_characters[i+1])) ))
        {
            /* find end of the number */
            int j;
            for (j = i + 1; j < in_length; j++)
            {
                if (!isdigit(in_characters[j]) && (in_characters[j] != '.'))
                    break;
            }
            *out_offset = i;
            *out_length = j - i;
            *out_concept = 19; // some arbitrary number we came up with for testing - should relate to a knowledge base
            return NL_OK;
        }
    }
    
    /* return no match */
    return NL_ESEARCH;
}


static NLTokenizer _tokenizer_numbers =
{
    plug_nl_numbers_startup,
    plug_nl_numbers_shutdown,
    plug_nl_numbers_get_next_token,
};


void nli_bi_plug_numbers_load(void)
{
    int nl_register_tokenizer(NLTokenizer *in_tokenizer);
    nl_register_tokenizer(&_tokenizer_numbers);
}

