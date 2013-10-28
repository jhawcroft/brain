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
/* natural language debug routines */

#include <stdio.h>

#include "common.h"


#if DEBUG == 1


void nl_input_debug(NLInput *in_input)
{
    printf("nl_input_debug():\n");
    
    /* enumerate all sentences */
    for (int i = 0; i < array_count(in_input->sentences); i++)
    {
        NLSentence *sentence = array_item(in_input->sentences, i);
        printf("Sentence %d:\n", (i+1));
        
        /* enumerate tokens */
        for (int t = 0; t < sentence->token_count; t++)
        {
            NLToken *token = sentence->tokens[t];
            printf("Token %d: flags=%04X, chars=\"%s\" (len=%d)\n", (t+1), token->flags, token->characters, token->length);
        }
    }
}



#endif