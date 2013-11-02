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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "conf.h"
#include "../protocol.h"
#include "../util/util.h"
#include "../mem/alloc.h"
#include "../util/sbuff.h"
#include "log.h"
#include "err.h"

#include "kn.h"
#include "nl.h"


static void handle_text(connection_t *in_conn, char const *in_text)
{
    int err;
    nlmeaning_t **meanings;
    int meaning_count;
    
    printf("User: %s\n", in_text);
    
    err = nl_input_to_meanings(in_text, &meanings, &meaning_count);
    if (err != NL_OK)
    {
        server_respond(in_conn, BRAIN_COMM_ERRR, NULL, 0);
        return;
    }
    
    for (int m = 0; m < meaning_count; m++)
    {
        nlmeaning_t *meaning = meanings[m];
        Sbuff *invokation = sbuff_create(SBUFF_DEFAULT_CAPACITY);
        
        printf("meaning: %s(", meaning->meaning);
        for (int a = 0; a < meaning->argument_count; a++)
        {
            printf("%s", meaning->arguments[a]);
            if (a + 1 < meaning->argument_count)
                printf(", ");
        }
        printf(")\n");
        
        sbuff_append_cstring(invokation, g_braind_thought_path, SBUFF_AUTO_LENGTH);
        sbuff_append_cstring(invokation, " ", 1);
        sbuff_append_cstring(invokation, meaning->meaning, SBUFF_AUTO_LENGTH);
        for (int a = 0; a < meaning->argument_count; a++)
        {
            sbuff_append_cstring(invokation, " ", 1);
            if (meaning->arguments[a])
                sbuff_append_cstring(invokation, meaning->arguments[a], SBUFF_AUTO_LENGTH);
            else
                sbuff_append_cstring(invokation, ".", SBUFF_AUTO_LENGTH);
        }
        printf("%s\n", sbuff_cstring(invokation));
        
        system(sbuff_cstring(invokation));
        sbuff_dispose(invokation);
        
        for (int a = 0; a < meaning->argument_count; a++)
        {
            if (meaning->arguments[a]) brain_free_(meaning->arguments[a]);
        }
        brain_free_(meaning);
    }
    if (meanings)
        brain_free_(meanings);
}



void despatch_request(connection_t *in_conn, int in_type, void *in_data, int in_size)
{
    if (in_type == BRAIN_COMM_IDLE)
        server_respond(in_conn, 0, NULL, 0);
    
    switch (in_type)
    {
        case BRAIN_COMM_TEXT:
            handle_text(in_conn, in_data);
            break;
    }
    
    
    /*printf("REQ %d:\"%s\" (%d)\n", in_type, (char*)in_data, in_size);
    if (in_type == 4)
        server_respond(in_conn, 9, "IDLE!", 6);*/
}






