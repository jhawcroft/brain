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
/* brain daemon logger */

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"


#define BRAIND_SYSLOG_IDENT "braind"


static FILE *g_log_file = NULL;

static int g_syslog_facility = LOG_DAEMON;


void log_init(char const *in_log_name, int in_syslog_local)
{
    /* configure syslog */
    switch (in_syslog_local)
    {
        case 0: g_syslog_facility = LOG_LOCAL0; break;
        case 1: g_syslog_facility = LOG_LOCAL0; break;
        case 2: g_syslog_facility = LOG_LOCAL0; break;
        case 3: g_syslog_facility = LOG_LOCAL0; break;
        case 4: g_syslog_facility = LOG_LOCAL0; break;
        case 5: g_syslog_facility = LOG_LOCAL0; break;
        case 6: g_syslog_facility = LOG_LOCAL0; break;
        case 7: g_syslog_facility = LOG_LOCAL0; break;
    }

    /* open connection to syslog;
     even if we use our own logger, this will allow us
     to do log something if that fails... */
    openlog(BRAIND_SYSLOG_IDENT, LOG_CONS, g_syslog_facility);
    
    if (!in_log_name) return; /* custom log file disabled;
                               just use syslog */
    
    /* try and open our own log file */
    g_log_file = fopen(in_log_name, "a");
    if (!g_log_file)
    {
        int err = errno;
        switch (err)
        {
            case EACCES:
                if (isatty(fileno(stdout)))
                    fprintf(stdout, "couldn't create log file; insufficient privileges\n");
                syslog(LOG_MAKEPRI(g_syslog_facility, LOG_ERR), "couldn't create log file; insufficient privileges");
                break;
            default:
                if (isatty(fileno(stdout)))
                    fprintf(stdout, "couldn't create log file; system error %d\n", err);
                syslog(LOG_MAKEPRI(g_syslog_facility, LOG_ERR), "couldn't create log file; system error %d", err);
                break;
        }
    }
}


void lvprintf(int in_level, char const *in_message, va_list in_args)
{
    switch (in_level)
    {
        case BRAIN_WARNING:
            if (g_log_file == NULL)
                vsyslog(LOG_MAKEPRI(g_syslog_facility, LOG_WARNING), in_message, in_args);
            else
                fprintf(g_log_file, "Warning: ");
            break;
        case BRAIN_ERROR:
            if (g_log_file == NULL)
                vsyslog(LOG_MAKEPRI(g_syslog_facility, LOG_ERR), in_message, in_args);
            else
                fprintf(g_log_file, "Fatal Error: ");
            break;
        case BRAIN_NOTICE:
            if (g_log_file == NULL)
                vsyslog(LOG_MAKEPRI(g_syslog_facility, LOG_NOTICE), in_message, in_args);
            break;
    }
    if (g_log_file != NULL)
        vfprintf(g_log_file, in_message, in_args);
}


void lprintf(int in_level, char const *in_message, ...)
{
    va_list arg_list;
    va_start(arg_list, in_message);
    lvprintf(in_level, in_message, arg_list);
    va_end(arg_list);
}


