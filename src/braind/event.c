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
#include <ctype.h>

#include "server.h"
#include "conf.h"
#include "../../includes/protocol.h"
#include "../util/util.h"
#include "../mem/alloc.h"
#include "../util/sbuff.h"
#include "../fatal.h"
#include "log.h"

#include "kn.h"
#include "nl.h"


extern char *g_brain_bin_thought;


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
        
        sbuff_append_cstring(invokation, g_brain_bin_thought, SBUFF_AUTO_LENGTH);
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


static char const* find_space(char const *in_string)
{
    while (*in_string && (!isspace(*in_string))) in_string++;
    return in_string;
}


static void handle_gen(connection_t *in_conn, char const *in_data)
{
    printf("Gen: %s\n", in_data);
    /* need to split data into words here;
     first word is meaning name, each of the others is an argument */
    
    char const *end = find_space(in_data);
    long len = end - in_data;
    if (len == 0) return;
    
    nlmeaning_t *meaning = brain_alloc_(sizeof(nlmeaning_t), 0);
    if (!meaning) brain_fatal_("out of memory");
    memset(meaning, 0, sizeof(nlmeaning_t));
    
    meaning->meaning = brain_strndup(in_data, len);
    if (!meaning->meaning) brain_fatal_("out of memory");
    
    in_data += len;
    while (isspace(*in_data))
    {
        in_data++;
        end = find_space(in_data);
        len = end - in_data;
        if (len == 0) break;
        
        char **new_args = brain_realloc_(meaning->arguments, sizeof(char*) * (meaning->argument_count + 1), 0);
        if (!new_args) brain_fatal_("out of memory");
        meaning->arguments = new_args;
        
        char *arg = meaning->arguments[meaning->argument_count++] = brain_strndup(in_data, len);
        if (!arg) brain_fatal_("out of memory");
        in_data += len;
    }
    
    /*printf("meaning: %s(", meaning->meaning);
    for (int a = 0; a < meaning->argument_count; a++)
    {
        printf("%s", meaning->arguments[a]);
        if (a + 1 < meaning->argument_count)
            printf(", ");
    }
    printf(")\n");*/
    
    char *the_output;
    nlmeaning_t *gen_meanings[1];
    gen_meanings[0] = meaning;
    int err = nl_meanings_to_output(gen_meanings, 1, &the_output, 0);
    if (err != NL_OK)
    {
        server_respond(in_conn, BRAIN_COMM_ERRR, NULL, 0);
    }
    else
    {
        printf("Output: %s\n", the_output);
        server_broadcast(BRAIN_COMM_TEXT, the_output, (int)strlen(the_output) + 1);
        if (the_output) brain_free_(the_output);
    }
    
    
    for (int a = 0; a < meaning->argument_count; a++)
    {
        if (meaning->arguments[a]) brain_free_(meaning->arguments[a]);
    }
    brain_free_(meaning);
    
    /*
     nlmeaning_t *gen_meanings[1];
     

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
    */
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
        case BRAIN_COMM_GENL:
            handle_gen(in_conn, in_data);
            break;
        case BRAIN_COMM_CKIE:
            if (in_size > 1)
            {
                printf("Got cookie: %s\n", in_data);
                if (in_conn->id_cookie) brain_free_(in_conn->id_cookie);
                in_conn->id_cookie = brain_strdup(in_data);
            }
            break;
    }
    
    
    /*printf("REQ %d:\"%s\" (%d)\n", in_type, (char*)in_data, in_size);
    if (in_type == 4)
        server_respond(in_conn, 9, "IDLE!", 6);*/
}






