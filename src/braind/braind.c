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
#include <errno.h>

#include "../config.h"
#include "../util/conf.h"
#include "../util/util.h"
#include "../fatal.h"

#include "../../includes/client.h"

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


extern int g_server_shutdown;
extern int g_server_restart;


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



static void write_pid_file(void)
{
    FILE *pid_file = fopen(g_brain_pid_name, "w");
    if (!pid_file) brain_fatal_("Couldn't write PID to file %s", g_brain_pid_name);
    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);
}


static pid_t read_pid_file(void)
{
    FILE *pid_file = fopen(g_brain_pid_name, "r");
    if (!pid_file) brain_fatal_("Couldn't read PID from file %s", g_brain_pid_name);
    pid_t result;
    fscanf(pid_file, "%d", &result);
    return result;
}


/* returns -1 if we're unsure, 0 if it's not and 1 if it is */
static int is_braind_running(void)
{
    brain_client_t *client;
    
    if (brain_client_configure(NULL))
        brain_fatal_("Couldn't read brain.conf.\n");
    
    client = brain_client_create(NULL, NULL);
    if (!client)
        brain_fatal_("Not enough memory.");
    
    int err = brain_client_connect(client);
    int result;
    if (err == 0) result = 1;
    else if ((err == BRAIN_ECONN) || (err == BRAIN_ESYS)) result = 0;
    else result = -1;
    
    brain_client_dispose(client);
    
    return result;
}


static void do_send_signal(void)
{
    /* find out about the current braind daemon (if any) */
    int is_running = is_braind_running();
    pid_t pid = 0;
    if (is_running > 0) pid = read_pid_file();
    
    /* decide what to do */
    switch (g_signal)
    {
        case BRAIN_SIGSTART:
            /* if braind is running, do nothing,
             otherwise, start braind with current arguments */
            if (is_running > 0)
            {
                printf("BRAIN is already running.\n");
                break;
            }
            return; /* skip exit() and continue executing main() */
            
        case SIGQUIT:
            /* if running, send kill SIGQUIT.
             otherwise output error */
            if (is_running > 0)
            {
                printf("Telling BRAIN to quit.\n");
                kill(pid, SIGQUIT);
            }
            else
                printf("BRAIN is not running.\n");
            break;
            
        case SIGTERM:
            /* if running, send kill SIGTERM.
             otherwise output error */
            if (is_running > 0)
            {
                printf("Terminating BRAIN.\n");
                kill(pid, SIGTERM);
            }
            else
                printf("BRAIN is not running.\n");
            break;
        case SIGUSR1:
            /* if running, send SIGUSR1.
             otherwise output error */
            if (is_running > 0)
            {
                printf("Telling BRAIN to restart.\n");
                kill(pid, SIGUSR1);
            }
            else
                printf("BRAIN is not running.\n");
            break;
    }
    exit(EXIT_SUCCESS);
}


static void handle_signal_(int in_signal)
{
    switch (in_signal)
    {
        case SIGQUIT:
            /* set a boolean that the event loop in 
             server.c will see and cause graceful shutdown */
            g_server_shutdown = 1;
            break;
        case SIGUSR1:
            /* ideally need to re-execve with the same environment
             and arguments we were initially created */
            g_server_restart = 1;
            g_server_shutdown = 1;
            break;
    }
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
    
    /* install signal handlers */
    signal(SIGQUIT, handle_signal_);
    signal(SIGUSR1, handle_signal_);
    
    /* write PID file */
    write_pid_file();
    
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
#if DEBUG == 1
    printf("Debugging      : Enabled\n");
#else
    printf("Debugging      : Not Available\n");
#endif
}


static void do_help(void)
{
    printf("Usage: braind [options]\n");
    printf("Options:\n");
    printf("  -s, --signal          Send signal to braind.  The argument can\n");
    printf("                        be one of: start, shutdown, stop and restart.\n");
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
        else if ((strcmp(optarg, "shutdown") == 0) || (strcmp(optarg, "quit") == 0))
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
    char *save_wd = getwd(NULL);
    
    process_options(argc, argv);
    
    if (g_signal)
    {
        do_send_signal();
    }

    start_daemon(); /* only if the command line arguments request it */
    
    /* if we get back here; we should restart
     with our original arguments and environment */
    if (chdir(save_wd))
    {
        brain_fatal_("Restart failed - system error on chdir %s: %d", save_wd, errno);
        exit(EXIT_FAILURE);
    }
    if (execvp(argv[0], (char**)argv))
    {
        brain_fatal_("Restart failed - system error on execvp %d", errno);
        exit(EXIT_FAILURE);
    }
    
    return 0; /* keep compiler happy */
}

