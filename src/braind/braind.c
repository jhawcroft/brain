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
/* brain daemon; primary server process that services requests to translate
 between natural language and meanings, and visa-versa. */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <libgen.h>

#include "../util/conf.h"
#include "../util/util.h"
#include "../fatal.h"
#include "log.h"

#include "kn.h"
#include "nl.h"


char *g_brain_bin_thought = NULL;


void brain_uds_start(void);


/* handle fatal errors */
void brain_fatal_(char const *in_message, ...)
{
    va_list args;
    va_start(args, in_message);
    lvprintf(BRAIN_ERROR, in_message, args);
    va_end(args);
    exit(EXIT_FAILURE);
}


static void daemonize(void)
{
    pid_t pid = fork();
    if (pid < 0) brain_fatal_("unable to fork() to background");
    if (pid > 0)
    {
        /* exit the parent process,
         returning control to the waiting caller */
        exit(EXIT_SUCCESS);
    }
    
    /* set child process file creation mask;
     rw- (file) r-x (directory) */
    umask(0);
    
    /* create a new session ID for the child process */
    if (setsid() < 0) brain_fatal_("unable to obtain session ID");
    
    /* change the current working directory */
    if (chdir("/") < 0) brain_fatal_("unable to change to root directory");
    
    /* close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


static void start_daemon(void)
{
    /* initalize basic logging to screen/syslog
     until configuration loaded */
    log_init(NULL, SYSLOG_DAEMON);
    
    /* load the configuration file */
    if (brain_configure_(NULL))
        brain_fatal_("Couldn't load brain.conf.\n");
    
    g_brain_bin_thought = brain_strdup(make_name(g_brain_bin, "thought"));
    if (!g_brain_bin_thought)
        brain_fatal_("Not enough memory.\n");
    
    /* daemonize (detatch from calling process) */
    daemonize();
    
    /* initalize the knowledge network
     and natural language processing engine */
    if (kn_startup()) brain_fatal_("couldn't initalize knowledge network");
    if (nl_startup()) brain_fatal_("couldn't initalize natural language engine");
    
    /* start the request handling service */
    brain_uds_start();
}


int main(int argc, const char * argv[])
{    
    start_daemon(); /* only if the command line arguments request it */
    
    
    
    
    return 0;
}

