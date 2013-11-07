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
/*
 thought: command-line utility; responsible for executing, logging and
 monitoring an individual 'thought'
 
 a thought is a script or program executable that takes zero or more
 arguments and may be written in any language.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>

#include "../config.h"
#include "../util/conf.h"
#include "../util/util.h"

extern char **environ;


/* program defaults */
#define DEFAULT_TIMEOUT_SECONDS     15


/* program compile-time configuration */
#define THOUGHT_VERSION_STRING      "1.0"

#define OUTPUT_XFER_BUFFER_SIZE     4096
#define MAX_THOUGHT_ID_LENGTH       50
#define MIN_TIMEOUT_SECONDS         2

#define LOG_PREFIX_FORMAT           "%s.%010ld"


/* basic thought information */
static char thought_id[MAX_THOUGHT_ID_LENGTH];
static char *thought_name = NULL;

/* configured file names/paths */
//static char *g_log_dir = NULL;

/* log file (if any) */
static FILE *log_file = NULL;

static char *g_id_cookie = NULL;

/* child process and communication pipe */
static pid_t g_script_pid;
static int g_fd_output;

/* current execution timeout */
static long g_timeout_secs = DEFAULT_TIMEOUT_SECONDS;

/* command-line switches */
static int g_debug = 0;
static int g_print_version = 0;
static int g_print_help = 0;



static void log_begin(void)
{
    if (g_debug)
        log_file = stdout;
    else
        log_file = fopen(g_brain_log_name, "a");
}


static void generate_thought_id(void)
{
    sprintf(thought_id, "%d", getpid());
}


static unsigned long time_elapsed(void)
{
    return clock() / (CLOCKS_PER_SEC / 1000);
}


static void thought_setup(void)
{
    /* generate a unique thought ID from the time */
    generate_thought_id();
    
    if (!g_debug)
    {
        /* 'daemonize' the current process */
        pid_t pid = fork();
        if (pid < 0)
        {
            if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Unable to go to background, aborting.\n",
                                  thought_id, time_elapsed());
            exit(EXIT_FAILURE);
        }
        if (pid > 0)
        {
            /* exit the parent process,
             returning control to the waiting caller */
            exit(EXIT_SUCCESS);
        }
    }
    
    /* set child process file creation mask;
     rw- (file) r-x (directory) */
    umask(0);
    
    /* create/open a log file */
    log_begin();
    
    if (!g_debug)
    {
        /* create a new session ID for the child process */
        if (setsid() < 0) {
            if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Unable to obtain session ID, aborting.\n",
                                  thought_id, time_elapsed());
            exit(EXIT_FAILURE);
        }
    }
    
    /* change the current working directory */
    if (chdir(g_brain_thoughts) < 0) {
        if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Unable to access thought directory, aborting.\n",
                              thought_id, time_elapsed());
        exit(EXIT_FAILURE);
    }
    
    if (!g_debug)
    {
        /* close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
}


static void thought_exec(int argc, char const *argv[])
{
    /* log the thought */
    if (log_file)
    {
        fprintf(log_file, LOG_PREFIX_FORMAT ": Thinking %s: ", thought_id, time_elapsed(), thought_name);
        for (int a = 2; a < argc; a++)
        {
            fprintf(log_file, "%s", argv[a]);
            if (a + 1 < argc) fprintf(log_file, ", ");
        }
        fprintf(log_file, "\n");
    }
    
    /* prepare arguments */
    char **child_args;
    int actual_argc = argc - optind;
    child_args = calloc(actual_argc, sizeof(char*));
    if (!child_args)
    {
        if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Not enough memory preparing arguments.\n",
                              thought_id, time_elapsed());
        exit(EXIT_FAILURE);
    }
    for (int a = optind; a < argc; a++)
    {
        child_args[a - optind] = (char*)argv[a];
    }
    
    /* create a pipe to receive output from the child */
    int pipedes[2];
    if (pipe(pipedes))
    {
        if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Couldn't create a pipe to the thought process.\n",
                              thought_id, time_elapsed());
        exit(EXIT_FAILURE);
    }
    if (fcntl(pipedes[0], F_SETFL, O_NONBLOCK))
    {
        if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Couldn't configure pipe to the thought process.\n",
                              thought_id, time_elapsed());
        exit(EXIT_FAILURE);
    }
    
    /* flush buffer so the child doesn't end up flushing it too */
    if (log_file) fflush(log_file);
    
    /* fork the thought process */
    g_script_pid = fork();
    if (g_script_pid < 0)
    {
        if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Couldn't create the thought process.\n",
                              thought_id, time_elapsed());
        exit(EXIT_FAILURE);
    }
    if (g_script_pid > 0)
    {
        /* monitor cleanup */
        close(pipedes[1]);
        g_fd_output = pipedes[0];
        free(child_args);
        return;
    }
    
    /* connect the pipe to stdout */
    close(pipedes[0]);
    dup2(pipedes[0], STDOUT_FILENO);
    fclose(log_file);
    log_file = NULL;
    
    /* execute the thought process */
    if (execve(child_args[0], child_args, environ))
    {
        switch (errno)
        {
            case E2BIG:
            case ENAMETOOLONG:
            case ENOTDIR:
                fprintf(stdout, "Arguments of thought are too long for this system.\n");
                break;
            case EACCES:
                fprintf(stdout, "Insufficient priviledges to execute thought process.\n");
                break;
            case ENOENT:
                fprintf(stdout, "Thought process cannot be found.\n");
                break;
            case ENOMEM:
                fprintf(stdout, "Insufficient memory to execute thought.\n");
                break;
            default:
                fprintf(stdout, "Thought execution failed because of an unknown %d error.\n", errno);
                break;
        }
        exit(EXIT_FAILURE);
    }
    /* successful execution begins in child main();
     we never get here */
}


static int has_output(void)
{
    fd_set des;
    struct timeval timeout;
    FD_ZERO(&des);
    FD_SET(g_fd_output, &des);
    timeout.tv_sec = 0;
    timeout.tv_usec = 10;
    
    int result = select(1, &des, NULL, NULL, &timeout);
    
    return (result > 0);
}


static int has_terminated(void)
{
    int stat_lock = 0;
    int result = waitpid(g_script_pid, &stat_lock, WNOHANG);
    return (result == g_script_pid);
}


static ssize_t line_size(char const *in_string, char const *in_limit)
{
    char const *ptr = in_string;
    while ((ptr != in_limit) && (*ptr != '\n') && (*ptr != '\r')) ptr++;
    if (ptr == in_limit) return 0;
    else return ptr + 1 - in_string;
}


#undef OUTPUT_XFER_BUFFER_SIZE
#define OUTPUT_XFER_BUFFER_SIZE 4096

static void thought_monitor(void)
{
    time_t start_time = time(NULL);
    char buffer[OUTPUT_XFER_BUFFER_SIZE];
    ssize_t buffer_size = 0;
    
    for (;;)
    {
        /* check if the thought process has terminated */
        int terminate = has_terminated();
        
        /* check for output from the thought process;
         copy it line-by-line to the log file or screen (if debugging) */
        if (has_output() || (buffer_size > 0))
        {
            for (;;)
            {
                ssize_t bytes_read = read(g_fd_output, buffer + buffer_size, OUTPUT_XFER_BUFFER_SIZE);
                if (bytes_read < 1) break;
                
                buffer_size += bytes_read;
                while (buffer_size > 0)
                {
                    ssize_t line_bytes = line_size(buffer, buffer + buffer_size);
                    
                    /* if we've filled the buffer, we have to output it,
                     regardless of whether we've found an end of line marker */
                    if ((line_bytes == 0) && (buffer_size == OUTPUT_XFER_BUFFER_SIZE))
                        line_bytes = buffer_size;
                    if ((terminate) && (line_bytes == 0))
                        line_bytes = buffer_size;
                    
                    /* can't see end of line, come back later */
                    if (line_bytes == 0) break;
    
                    /* output the log line with an appropriate prefix */
                    if (log_file)
                    {
                        fprintf(log_file, LOG_PREFIX_FORMAT ": ", thought_id, time_elapsed());
                        fwrite(buffer, 1, line_bytes, log_file);
                    }
                    
                    /* adjust the buffer so we don't consider this line again */
                    memmove(buffer, buffer + line_bytes, buffer_size - line_bytes);
                    buffer_size -= line_bytes;
                }
            }
        }
        
        /* exit if the process has terminated */
        if (terminate) break;
        
        /* otherwise check if the process has been running too long */
        else if (time(NULL) - start_time >= g_timeout_secs)
        {
            kill(g_script_pid, SIGKILL);
            if (log_file) fprintf(log_file, LOG_PREFIX_FORMAT ": Thought took too long and was aborted.\n",
                                  thought_id, time_elapsed());
            break;
        }
    
        /* sleep for a while */
        usleep(500);
    }
}


static void do_version(void)
{
    printf("thought (%s) %s\n", PACKAGE_STRING, THOUGHT_VERSION_STRING);
    printf("Copyright (C) 2012-2013 Joshua Hawcroft\n");
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}


static void do_help(void)
{
    printf("Usage: thought [options] name ...\n");
    printf("Options:\n");
    printf("  -d, --debug           Debug mode; thought output is directed to \n");
    printf("                        the screen rather than the designated log.\n");
    printf("  -t, --timeout         Causes the thought to be aborted if it \n");
    printf("                        takes longer than the specified number of\n");
    printf("                        seconds.\n");
    printf("  -C, --cookie          Supply an identification token to the\n");
    printf("                        service to identify this session.\n");
    printf("  -v, --version         Print version of thought and exit.\n");
    printf("  -h, --help            Prints this help text and exits.\n");
    printf("\n");
    printf("Report bugs to: %s\n", PACKAGE_BUGREPORT);
    printf("BRAIN home page: <%s>\n", PACKAGE_URL);
}


static struct option long_options[] =
{
    {"debug",   no_argument,       &g_debug,            1},
    {"version", no_argument,       &g_print_version,    1},
    {"help",    no_argument,       &g_print_help,       1},

    {"timeout", required_argument, 0,                   't'},
    {"cookie",  required_argument, 0,                   'C'},
    
    {0, 0, 0, 0}
};


static char* short_options = "dvhtC:";


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
            
            case 'd':
                g_debug = 1;
                break;
            case 'h':
                g_print_help = 1;
                goto finish_processing_options;
            case 'v':
                g_print_version = 1;
                goto finish_processing_options;
            case 'C':
                use_specified_cookie();
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
    if ((g_print_help) || (argc == 1))
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
#ifdef DEBUG
    printf("DEBUG MODE\n");
#endif
    
    process_options(argc, argv);
    
    thought_name = brain_strdup(argv[optind]);
    basename(thought_name);
    
    if (brain_configure_(NULL))
    {
        fprintf(stderr, "Couldn't read %s\n", g_brain_conf_name);
        exit(EXIT_FAILURE);
    }
    
    if (g_id_cookie)
        setenv("BRAIN_COOKIE", g_id_cookie, 1);
    
    thought_setup();
    thought_exec(argc, argv);
    thought_monitor();
    
    exit(EXIT_SUCCESS);
    return 0; /* keep the compiler happy */
}

