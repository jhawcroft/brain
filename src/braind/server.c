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
/* UNIX domain socket server implementation */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>

#include "err.h"
#include "log.h"
#include "conf.h"
#include "server.h"

#include "../protocol.h"


/* compile-time configuration */
#define SERVER_SLEEP_SECONDS 2

#define BRAIND_NAME         "braind"
#define BRAIND_VERSION      "1.0"


/* globals I need to patch in from configuration: */
int g_max_connections = 50;
int g_connection_queue = 5;


int g_conn_buffer_size = 4096; /* MUST be at least 2 x the size of the longest request
                                our output to be generated (should be configurable) */


/* globals for this unit only: */
static int g_unix_listener = 0;
static int g_conn_count = 0;
static bool g_got_wakeup = false;
static int g_split_buff_size = 0;




static connection_t *g_connections = NULL;

/* suggest we just use a run-loop, with poll() and feed signal handling to the
 same function that is used to perform these checks?  that way overflows should
 be fine and we should be responsive due to the signal handler, but not running
 a loop too often hopefully... */


void server_respond(connection_t *in_conn, int in_type, void *in_data, int in_size);


static void accept_new_connections(void)
{
    /* check if there are too many connections */
    if (g_conn_count >= g_max_connections) return;
    
    for (;;)
    {
        /* try to accept an incoming connection */
        int conn_sock = accept(g_unix_listener, NULL, NULL);
        if (conn_sock < 0) return;
        
        /* find next unused connection slot */
        for (int i = 0; i < g_max_connections; i++)
        {
            if (g_connections[i].sock < 0)
            {
                /* configure the new connection record */
                g_connections[i].sock = conn_sock;
                g_connections[i].last_activity = time(NULL);
                g_connections[i].recv_size = 0;
                g_connections[i].writ_size = 0;
                g_connections[i].z_count = 0;
                
                /* make the connection socket non-blocking and asyncronous */
                if (fcntl(conn_sock, F_SETFL, O_ASYNC | O_NONBLOCK))
                {
                    fprintf(stdout, "Couldn't make connection socket asyncronous.\n");
                    exit(EXIT_FAILURE);
                }
                
                /* send a welcome banner */
                server_respond(g_connections + i,
                               BRAIN_COMM_HELO,
                               BRAIND_NAME " " BRAIND_VERSION,
                               sizeof(BRAIND_NAME " " BRAIND_VERSION));
                
                /* try and accept another connection */
                conn_sock = -1;
                break;
            }
        }
        if (conn_sock < 0) continue;
        
        /* shouldn't get here;
         no connection was found */
        fprintf(stdout, "Internal error - accepting connection but connection table is full.\n");
        return;
    }
}


static void kill_conn(connection_t *in_conn)
{
    close(in_conn->sock);
    in_conn->sock = -1;
}


void server_respond(connection_t *in_conn, int in_type, void *in_data, int in_size)
{
    printf("RPY %d:\"%s\" (%d)\n", in_type, (char*)in_data, in_size);
    int actual_size = in_size + 3;
    if (g_split_buff_size - in_conn->writ_size < actual_size)
    {
        fprintf(stderr, "Couldn't send response to user, buffer overflow.\n");
        return;
    }
    in_conn->writ_buffer[in_conn->writ_size] = in_type;
    in_conn->writ_buffer[in_conn->writ_size + 1] = (0xFF00 & in_size) >> 8;
    in_conn->writ_buffer[in_conn->writ_size + 2] = 0xFF & in_size;
    memcpy(in_conn->writ_buffer + in_conn->writ_size + 3, in_data, in_size);
    in_conn->writ_size += actual_size;
}


void server_broadcast(int in_type, void *in_data, int in_size)
{
    for (int c = 0; c < g_max_connections; c++)
    {
        if (g_connections[c].sock != -1)
        {
            server_respond(g_connections + c, in_type, in_data, in_size);
        }
    }
}




static void server_awake(void)
{
    g_got_wakeup = false;
    
    accept_new_connections();
    
    time_t now = time(NULL);
    for (int i = 0; i < g_max_connections; i++)
    {
        connection_t *conn = g_connections + i;
        if (conn->sock < 0) continue;
        
        /* check for zombie */
        if ((conn->z_count > 0) ||
            (now - conn->last_activity >= BRAIN_COMM_CONN_TIMEOUT_SECS))
            conn->z_count++;
        if (conn->z_count > 2) /* z_count ensures all connections have a chance to 
                                handle incoming/outgoing data before being considered
                                a zombie; in case one connection takes a long time
                                to handle it's data */
        {
            kill_conn(conn);
            continue;
        }
        
        /* check for closure, or data available/writable */
        struct pollfd poll_what;
        poll_what.fd = conn->sock;
        poll_what.events = POLLRDNORM | POLLWRNORM | POLLERR | POLLHUP;
        poll_what.revents = 0;
        if (poll(&poll_what, 1, 0) > 0)
        {
            if (poll_what.revents & (POLLERR | POLLHUP))
            {
                /* connection closed */
                kill_conn(conn);
            }
            else
            {
                if (poll_what.revents & POLLRDNORM)
                {
                    /* read incoming */
                    conn->last_activity = now;
                    conn->z_count = 0;
                    while (conn->recv_size < g_split_buff_size)
                    {
                        long bytes = read(conn->sock,
                                         conn->recv_buffer + conn->recv_size,
                                         g_split_buff_size - conn->recv_size);
                        if (bytes <= 0) break;
                        conn->recv_size += bytes;
                    }
                    
                    /* despatch requests */
                    while (conn->recv_size >= 3)
                    {
                        int req_type = conn->recv_buffer[0];
                        int req_size = (conn->recv_buffer[1] << 8) + conn->recv_buffer[2];
                        if ((req_size < 0) || (req_size > g_split_buff_size))
                        {
                            fprintf(stderr, "Connection receive buffer overflow, closing connection.\n");
                            kill_conn(conn);
                            break;
                        }
                        int actual_size = 3 + req_size;
                        if (conn->recv_size < actual_size) break;
                        despatch_request(conn, req_type, conn->recv_buffer + 3, req_size);
                        
                        /* TODO: this could eventually use a ring buffer to avoid
                         moving memory all the time */
                        memmove(conn->recv_buffer,
                                conn->recv_buffer + actual_size,
                                conn->recv_size - actual_size);
                        conn->recv_size -= actual_size;
                    }
                }
                if ((poll_what.revents & POLLWRNORM) && (conn->writ_size > 0))
                {
                    /* write outgoing */
                    conn->last_activity = now;
                    conn->z_count = 0;
                    while (conn->writ_size > 0)
                    {
                        long bytes = write(conn->sock, conn->writ_buffer, conn->writ_size);
                        if (bytes <= 0) break;
                        memmove(conn->writ_buffer,
                                conn->writ_buffer + bytes,
                                conn->writ_size - bytes);
                        conn->writ_size -= bytes;
                    }
                }
            }
        }
    }
}


static void server_runloop(void)
{
    for (;;)
    {
        if (!g_got_wakeup)
            sleep(SERVER_SLEEP_SECONDS);
        server_awake();
    }
}



/* dummy SIGIO handler to ensure we awake from sleep()
 may be able to use SIGPIPE handler to get rid of buggered connections more quickly?
 */
/*static void handle_sigio_(int in_signal, siginfo_t *in_info, void *in_context)
{
    printf("Got SIGIO\n");
    return;
}

*/

/* dummy SIGIO handler to ensure we awake from sleep() reliably */
static void handle_sigio2_(int in_signal)
{
    g_got_wakeup = true;
    return;
}


void brain_uds_start(void)
{
    /* create a pool of connection records */
    g_split_buff_size = g_conn_buffer_size / 2;
    g_connections = calloc(g_max_connections, sizeof(connection_t));
    if (!g_connections)
    {
        fprintf(stdout, "Not enough memory to start server.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < g_max_connections; i++)
    {
        g_connections[i].sock = -1;
        g_connections[i].recv_buffer = malloc(g_split_buff_size);
        if (!g_connections[i].recv_buffer)
        {
            fprintf(stdout, "Not enough memory to start server.\n");
            exit(EXIT_FAILURE);
        }
        g_connections[i].writ_buffer = malloc(g_split_buff_size);
        if (!g_connections[i].writ_buffer)
        {
            fprintf(stdout, "Not enough memory to start server.\n");
            exit(EXIT_FAILURE);
        }
    }
    
    /* create a socket */
    g_unix_listener = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_unix_listener < 0)
    {
        fprintf(stdout, "Couldn't create listening socket.\n");
        exit(EXIT_FAILURE);
    }
    
    /* bind to the local address */
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    if (strlen(g_braind_server_sock) > sizeof(local_addr.sun_path)-1)
    {
        fprintf(stdout, "Listening socket name is too long.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(local_addr.sun_path, g_braind_server_sock);
    unlink(g_braind_server_sock);
    if (bind(g_unix_listener, (struct sockaddr*)&local_addr, sizeof(struct sockaddr_un)))
    {
        switch (errno)
        {
            case EACCES:
                fprintf(stdout, "Couldn't bind - insufficient privileges.\n");
                break;
            case EADDRINUSE:
                fprintf(stdout, "Couldn't bind - the address is already in use.\n");
                break;
            default:
                fprintf(stdout, "Couldn't bind - unknown system error %d\n", errno);
                break;
        }
        exit(EXIT_FAILURE);
    }
    
    /* listen for incoming connections */
    if (listen(g_unix_listener, g_connection_queue))
    {
        fprintf(stdout, "Couldn't listen - unknown system error %d\n", errno);
        exit(EXIT_FAILURE);
    }
    
    /* ask for a SIGIO when a connection is pending
     and make listener non-blocking */
    if (fcntl(g_unix_listener, F_SETFL, O_ASYNC | O_NONBLOCK))
    {
        fprintf(stdout, "Couldn't make listening socket asyncronous.\n");
        exit(EXIT_FAILURE);
    }
    if (fcntl(g_unix_listener, F_SETOWN, getpid()))
    {
        fprintf(stdout, "Couldn't make listening socket signalling.\n");
        exit(EXIT_FAILURE);
    }
    
    /* set the SIGIO handler */
    signal(SIGIO, &handle_sigio2_);
    /*struct sigaction accept_action;
    accept_action.sa_flags = 0;
    accept_action.sa_mask = 0;
    accept_action.sa_sigaction = &handle_sigio_;
    if (sigaction(SIGIO, &accept_action, NULL))
    {
        fprintf(stdout, "Couldn't configure signalling.\n");
        exit(EXIT_FAILURE);
    }*/
    
    /* run the server event loop */
    fprintf(stdout, "Started BRAIN server.\n");
    server_runloop();
}









