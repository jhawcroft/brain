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
/* development test program */

#include <stdio.h>
#include <stdlib.h>

#include "nl.h"
#include "kn.h"


void fatal(char const *in_message)
{
    printf("fatal error: %s\n", in_message);
    abort();
}


int main(int argc, const char * argv[])
{
    // need to prepare allocator here **TODO**
    
    
    if (nl_startup()) fatal("couldn't start NL");
    if (kn_startup()) fatal("couldn't start KN");
    
    //void test_nli_parse(void);
    //test_nli_parse();
    //int nl_input_to_meanings(char const *in_utterance, nlmeaning_t **out_meanings[], int *out_count)
    
    int err;
    nlmeaning_t **meanings;
    int meaning_count;
    
    err = nl_input_to_meanings("I really enjoy chicken!", &meanings, &meaning_count);
    //err = nl_input_to_meanings("hi josh!", &meanings, &meaning_count);
    if (err == NL_OK)
    {
        for (int m = 0; m < meaning_count; m++)
        {
            nlmeaning_t *meaning = meanings[m];
            printf("meaning: %s ", meaning->meaning);
            for (int a = 0; a < meaning->argument_count; a++)
            {
                printf("%s", meaning->arguments[a]);
                if (a + 1 < meaning->argument_count)
                    printf(", ");
            }
            printf("\n");
        }
    }
    else
    {
        printf("ERROR: %d\n", err);
    }
    
    /* on language generation:
     
     when we come to generation, probably going to want something on specific representations
     of a concept to indicate that those representations are not appropriate for output;
     need to consider that somewhat - perhaps could be suggestions on the actual pattern tokens;
     specific to output generation? 
     
     output generation is largely experimental on this mechanism anyway. 
     
     may need to run up a signficant number of patterns through training before a way to tweak this
     particularly with respect to foriegn languages, becomes obvious */
    
    return 0;
}

