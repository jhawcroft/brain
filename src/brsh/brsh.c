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
/* brain shell; command-line interface to make requests of, and receive output
 from the BRAIN system */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#include "../config.h"
#include "../util/confscan.h"
#include "../util/util.h"

#include "../../includes/client.h"

#include "ui.h"
#include "../fatal.h"



brain_client_t *g_client = NULL;





/* program defaults */
#define DEFAULT_TIMEOUT_SECONDS     5


/* current execution timeout */
static long g_timeout_secs = DEFAULT_TIMEOUT_SECONDS;

/* command-line switches */
static int g_debug = 0;
static int g_print_version = 0;
static int g_print_help = 0;


/* globals used for client configuration */
char *g_braind_server_sock = BRAIND_SERVER_SOCK;

int g_conn_buffer_size = 4096; /* MUST be at least 2 x the size of the longest request
                                our output to be generated (should be configurable, see also server.c) */



static void meaning2nl(int argc, char const *argv[])
{
    long len = 0;
    for (int a = 0; a < argc; a++)
    {
        len += strlen(argv[a]) + 1;
    }
    char *data = malloc(len + 1);
    if (!data) brain_fatal_("not enough memory");
    len = 0;
    for (int a = 0; a < argc; a++)
    {
        if (a + 1 < argc)
            len += sprintf(data + len, "%s ", argv[a]);
        else
            len += sprintf(data + len, "%s", argv[a]);
    }

    brain_client_send_request(g_client, BRAIN_COMM_GENL, data, (int)strlen(data) + 1);
    free(data);
    brain_client_wait_for_send(g_client);
}


int main(int argc, const char *argv[])
{
    /* need to handle command line args */
    
    if (brain_client_configure(NULL))
        brain_fatal_("Couldn't read brain.conf.\n");

    g_client = brain_client_create(NULL, NULL);
    if (!g_client) brain_fatal_("Not enough memory.\n");
    
    int err;
    err = brain_client_connect(g_client);
    if (err)
    {
        switch (err)
        {
            case BRAIN_EMEMORY:
                brain_fatal_("Not enough memory.\n");
            case BRAIN_ECONN:
                brain_fatal_("BRAIN service is not running.\n");
            case BRAIN_ESYS:
                brain_fatal_("An unexpected system error occurred.\n");
            case BRAIN_EINTERNAL:
            default:
                brain_fatal_("An unexpected internal error occurred.\n");
        }
    }
    
    if (strstr(argv[0], "respond") != 0)
    {
        meaning2nl(argc - 1, argv + 1);
        return 0;
    }
    
    brsh();
    
    return 0;
}

