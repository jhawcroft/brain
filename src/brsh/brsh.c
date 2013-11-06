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
#include <libgen.h>

#include "../config.h"
#include "../util/confscan.h"
#include "../util/util.h"

#include "../../includes/client.h"

#include "ui.h"
#include "../fatal.h"



brain_client_t *g_client = NULL;
char *g_invoked_name = NULL;


/* compile-time configuration */
#define BRSH_VERSION_STRING "1.0"
#define MIN_TIMEOUT_SECONDS    2


/* program defaults */
#define DEFAULT_TIMEOUT_SECONDS     5


/* current execution timeout */
static long g_timeout_secs = DEFAULT_TIMEOUT_SECONDS;

/* identification cookie */
static char *g_id_cookie = NULL;


/* command-line switches */
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



static void do_version(void)
{
    printf("%s (BRAIN) %s\n", g_invoked_name, BRSH_VERSION_STRING);
    printf("Copyright (C) 2012-2013 Joshua Hawcroft\n");
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}


static void do_help(void)
{
    printf("Usage: %s [options] [thought ...]\n", g_invoked_name);
    printf("Options:\n");
    printf("  -C, --cookie          Supply an identification token to the\n");
    printf("                        service to identify this session.\n");
    printf("  -t, --timeout         Terminate if the connection remains idle\n");
    printf("                        for the specified seconds.\n");
    printf("  -v, --version         Print version of thought and exit.\n");
    printf("  -h, --help            Prints this help text and exits.\n");
    printf("\n");
    printf("Report bugs to: bugs@joshhawcroft.org\n");
    printf("BRAIN home page: <http://joshhawcroft.org/brain/\n");
}


static struct option long_options[] =
{
    {"version", no_argument,       &g_print_version,    1},
    {"help",    no_argument,       &g_print_help,       1},
    
    {"timeout", required_argument, 0,                   't'},
    {"cookie",  required_argument, 0,                   'C'},
    
    {0, 0, 0, 0}
};


static char* short_options = "vhtC:";


static void use_specified_timeout(void)
{
    int timeout = (optarg ? atoi(optarg): 0);
    if ((timeout > 0) && (timeout < MIN_TIMEOUT_SECONDS))
        timeout = MIN_TIMEOUT_SECONDS;
    else
        g_timeout_secs = timeout;
}


static void use_specified_cookie(void)
{
    if (optarg) g_id_cookie = strdup(optarg);
}


static void process_options(int argc, char const *argv[])
{
    int c;
    for (;;) /* iterate over command-line options */
    {
        int long_option_index = 0;
        c = getopt_long (argc, (char**)argv, short_options,
                         long_options, &long_option_index);
        
        if (c == -1) break; /* no more options */
        switch (c)
        {
                /* handle a long option */
            case 0:
            {
                if ((g_print_version) || (g_print_help))
                    goto finish_processing_options;
                
                /* switches have already been handled */
                if (long_options[long_option_index].flag != 0) break;
                
                switch (long_options[long_option_index].val)
                {
                    case 't':
                        use_specified_timeout();
                        break;
                    case 'C':
                        use_specified_cookie();
                        break;
                }
                break;
            }
                
                /* handle a short option */
            case 't':
                use_specified_timeout();
                break;
            case 'C':
                use_specified_cookie();
                break;
            case 'h':
                g_print_help = 1;
                goto finish_processing_options;
            case 'v':
                g_print_version = 1;
                goto finish_processing_options;
                
            case '?':
                /* getopt_long already printed an error message. */
                do_help();
                exit(EXIT_FAILURE);
                
            default:
                abort ();
        }
    }
    
finish_processing_options:
    if (g_print_help)
    {
        do_help();
        exit(EXIT_SUCCESS);
    }
    if (g_print_version)
    {
        do_version();
        exit(EXIT_SUCCESS);
    }
}



int main(int argc, const char *argv[])
{
    /* get the name we were invoked with */
    g_invoked_name = strdup(argv[0]);
    g_invoked_name = basename(g_invoked_name);
    
    /* check for identification cookie in environment variables */
    if (getenv("BRAIN_COOKIE"))
        g_id_cookie = strdup(getenv("BRAIN_COOKIE"));
    
    /* process command line arguments */
    process_options(argc, argv);
    
    /* configure */
    if (brain_client_configure(NULL))
        brain_fatal_("Couldn't read brain.conf.\n");

    /* connect to daemon */
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
    
    /* if there's an identification cookie, send it now */
    if (g_id_cookie)
        brain_client_send_request(g_client, BRAIN_COMM_CKIE, g_id_cookie, (int)strlen(g_id_cookie) + 1);
    
    /* if we're invoked in an automatic mode,
     interpret the commands given and exit */
    if (strstr(argv[0], "respond") != 0)
    {
        meaning2nl(argc - 1, argv + 1);
        return 0;
    }
    
    /* invoke the normal interactive shell */
    brsh();
    
    return 0;
}

