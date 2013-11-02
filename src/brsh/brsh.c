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
#include <errno.h>
#include <string.h>

#include "../config.h"
#include "../util/confscan.h"
#include "../util/util.h"

#include "ui.h"
#include "client.h"
#include "error.h"


#define CONFIG_OK 0
#define CONFIG_ERROR -1


char *g_braind_server_sock = BRAIND_SERVER_SOCK;


static int scan_conf_line_(long in_line_number, char const *in_key, char const *in_value)
{
    if (strcmp(in_key, "local-socket") == 0)
        g_braind_server_sock = brain_strdup(in_value);
    
    return CONFIG_OK;
}


static void configure(void)
{
    int err;
    char *config_file = brain_strdup(make_name(BRAIN_CONFIG_DIR, BRAIN_CONFIG_FILE));
    if (!config_file)
        fatal("not enough memory to load configuration file");
    if ( ( err = confscan(config_file, &scan_conf_line_)) )
    {
        switch (err)
        {
            case ENOENT:
                fatal("configuration file %s not found", config_file);
            case EACCES:
                fatal("insufficient privileges to access configuration file %s", config_file);
            default:
                fatal("error %d reading configuration file %s", err, config_file);
                break;
        }
        exit(EXIT_FAILURE);
    }
}


int main(int argc, const char * argv[])
{
    /* need to handle command line args */
    
    configure();

    
    client_connect();
    brsh();
    
    return 0;
}

