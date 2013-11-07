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
#include <signal.h>

#include "config.h"
#include "../util/conf.h"
#include "../util/util.h"
#include "../fatal.h"
#include "log.h"

#include "../kn/kn.h"
#include "../nl/nl.h"


/* the name of the thought executable;
 computed at startup based on configuration */
char *g_brain_bin_thought = NULL;


/* command-line switches */
static int g_debug = 0;
static int g_daemonize = 0;
static int g_print_version = 0;
static int g_print_help = 0;
static int g_signal = 0;


#define BRAIN_SIGSTART -1


/* entry to the listening service */
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



static void do_send_signal(void)
{
    printf("Sending signals not yet implemented!\n");
}



static void daemonize(void)
{
    log_deinit();
    
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
    
    /* reinitalize logging after fork() */
    log_init(g_brain_log_name, SYSLOG_DAEMON);
    if (g_debug) log_debug();
    
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
    log_stdout(1);
    
    /* load the configuration file */
    if (brain_configure_(NULL))
        brain_fatal_("Couldn't load brain.conf.\n");
    
    /* reinitalize logging in case the configuration specifies a log file */
    log_init(g_brain_log_name, SYSLOG_DAEMON);
    
    /* verify mandatory configuration */
    g_brain_bin_thought = brain_strdup(make_name(g_brain_bin, "thought"));
    if (!g_brain_bin_thought)
        brain_fatal_("Not enough memory.\n");
    
    /* daemonize (detatch from calling process) */
    if (g_daemonize)
    {
        daemonize();
    }
    else
        log_debug();
    
    /* initalize the knowledge network
     and natural language processing engine */
    if (kn_startup()) brain_fatal_("couldn't initalize knowledge network");
    if (nl_startup()) brain_fatal_("couldn't initalize natural language engine");
    
    /* start the request handling service */
    brain_uds_start();
}



static void do_version(void)
{
    printf("braind (%s)\n", PACKAGE_STRING);
    printf("Copyright (C) 2012-2013 Joshua Hawcroft\n");
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n\n");
    
    printf("Compiled " __DATE__ " " __TIME__ "\n");
#if HAVE_UNISTRING == 1
    printf("Unicode Support: GNU libunistring\n");
#else
    printf("Unicode Support: Not Available (see configure --with-unistring)\n");
#endif
}


static void do_help(void)
{
    printf("Usage: braind [options]\n");
    printf("Options:\n");
    printf("  -s, --signal          Send signal to braind.  The argument can\n");
    printf("                        be one of: start, stop, quit and restart.\n");
    printf("  -B, --background      Run in the background.\n");
    printf("  -d, --debug           Debug mode; enables very verbose logging.\n");
    printf("  -v, --version         Print version of thought and exit.\n");
    printf("  -h, --help            Prints this help text and exits.\n");
    printf("\n");
    printf("Report bugs to: %s\n", PACKAGE_BUGREPORT);
    printf("%s home page: <%s>\n", PACKAGE_NAME, PACKAGE_URL);
}


static struct option long_options[] =
{
    {"debug",   no_argument,       &g_debug,            1},
    {"background",  no_argument,   &g_daemonize,            1},
    {"version", no_argument,       &g_print_version,    1},
    {"help",    no_argument,       &g_print_help,       1},
    
    {"signal",  required_argument, 0,                   's'},
    
    {0, 0, 0, 0}
};


static char* short_options = "s:Bdvh";


static void use_specified_signal(void)
{
    if (optarg)
    {
        if (strcmp(optarg, "start") == 0)
            g_signal = BRAIN_SIGSTART;
        else if (strcmp(optarg, "stop") == 0)
            g_signal = SIGTERM;
        else if (strcmp(optarg, "quit") == 0)
            g_signal = SIGQUIT;
        else if (strcmp(optarg, "restart") == 0)
            g_signal = SIGUSR1;
        else
        {
            printf("Invalid signal - %s\n", optarg);
            do_help();
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        do_help();
        exit(EXIT_FAILURE);
    }
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
                    case 's':
                        use_specified_signal();
                        break;
                }
                break;
            }
                
                /* handle a short option */
            case 's':
                use_specified_signal();
                break;
                
            case 'd':
                g_debug = 1;
                break;
            case 'h':
                g_print_help = 1;
                goto finish_processing_options;
            case 'v':
                g_print_version = 1;
                goto finish_processing_options;
            case 'B':
                g_daemonize = 1;
                break;
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



int main(int argc, const char * argv[])
{
    process_options(argc, argv);
    
    if (g_signal)
    {
        do_send_signal();
        exit(EXIT_SUCCESS);
    }

    start_daemon(); /* only if the command line arguments request it */
    
    exit(EXIT_SUCCESS);
    return 0; /* keep compiler happy */
}

