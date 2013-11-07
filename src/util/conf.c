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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../config.h"
#include "../fatal.h"
#include "confscan.h"


/* load default configuration */
char const* g_brain_conf_name = NULL;
char const* g_brain_thoughts = BRAIN_THOUGHTS_DIR;
char const* g_brain_bin = BRAIN_BIN_DIR;
char const* g_brain_socket_name = BRAIN_SOCKET;
char const* g_brain_log_name = BRAIN_LOG;

int g_brain_buffer = BRAIN_BUFFER;



/* configuration file loader */

#define CONFIG_OK 0
#define CONFIG_ERROR -1


static int scan_conf_line_(long in_line_number, char const *in_key, char const *in_value)
{
    if (strcmp(in_key, "thoughts") == 0)
        g_brain_thoughts = strdup(in_value);
    else if (strcmp(in_key, "socket") == 0)
        g_brain_socket_name = strdup(in_value);
    else if (strcmp(in_key, "bin") == 0)
        g_brain_bin = strdup(in_value);
    else if (strcmp(in_key, "buffer") == 0)
        g_brain_buffer = atoi(in_value);
    
    return CONFIG_OK;
}


int brain_configure_(char const *in_alternate_config_file)
{
    char const *config_file = (in_alternate_config_file ? in_alternate_config_file : BRAIN_CONFIG);
    g_brain_conf_name = config_file;
    return brain_confscan_(config_file, &scan_conf_line_);
}






