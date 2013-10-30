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
#include <string.h>

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
    
    
    err = nl_input_to_meanings("I really really enjoy chicken!", &meanings, &meaning_count);
    //err = nl_input_to_meanings("hi josh!", &meanings, &meaning_count);
    if (err == NL_OK)
    {
        for (int m = 0; m < meaning_count; m++)
        {
            nlmeaning_t *meaning = meanings[m];
            printf("meaning: %s(", meaning->meaning);
            for (int a = 0; a < meaning->argument_count; a++)
            {
                printf("%s", meaning->arguments[a]);
                if (a + 1 < meaning->argument_count)
                    printf(", ");
            }
            printf(")\n");
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
     particularly with respect to foriegn languages, becomes obvious 
     
     could be an alternative concept for a pattern token for output; ie. pronoun instead of agent
     and emphasis on making sure patterns are reversable / having training routines that allow
     correction of incorrect usages
     */
    
    //int nl_meanings_to_output(nlmeaning_t *in_meanings[], int in_count, char **out_output, int in_hints);
    
    nlmeaning_t *gen_meanings[1];
    
    /*
    gen_meanings[0] = calloc(1, sizeof(nlmeaning_t));
    gen_meanings[0]->meaning = strdup("hello");
    gen_meanings[0]->argument_count = 1;
    gen_meanings[0]->arguments = calloc(gen_meanings[0]->argument_count, sizeof(char*));
    gen_meanings[0]->arguments[0] = strdup("greeting");
    */
    
    gen_meanings[0] = calloc(1, sizeof(nlmeaning_t));
    gen_meanings[0]->meaning = strdup("like-something");
    gen_meanings[0]->argument_count = 4;
    gen_meanings[0]->arguments = calloc(gen_meanings[0]->argument_count, sizeof(char*));
    gen_meanings[0]->arguments[0] = strdup("to-like");
    gen_meanings[0]->arguments[1] = NULL;
    gen_meanings[0]->arguments[2] = strdup("agent-writer");
    gen_meanings[0]->arguments[3] = strdup("beef");
     
    char *the_output;
    
    err = nl_meanings_to_output(gen_meanings, 1, &the_output, 0);
    if (err == NL_OK)
    {
        printf("output: %s\n", the_output);
    }
    else
    {
        printf("ERROR: %d\n", err);
    }
    
    
    // get basic generation up and running;
    
    // then we're on to packaging - wrapping as a daemon process,
    // and providing a means to link meaning strings & arguments to file-system
    // stored macros (which are just *nix executables).
    
    // the inputs could be provided either as command args or as standard input
    
    // the output would be standard output in all cases
    
    // the results would be returned to the caller of the daemon asyncronously with the input
    
    // a mechanism shall allow any *nix executable (possibly on a cron schedule)
    // to ask the daemon to invoke the appropriate meaning script for a specific meaning,
    // such that behaviours can also occur at idle and responses generated will go to whatever
    // connected listeners are around
    
    // use an orthagonal approach so there's only one server type & client-server protocol
    
    // whatever NL outputs are generated get distributed to all client connections
    // at this time
    
    // focus on bootstrap mechanisms & control commands via protocol to enable
    // training of the system's NL patterns rather than manual scripting/entry/maintenance
    
    
    
    return 0;
}

