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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>

#include "error.h"

/*
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
*/

extern char *g_braind_server_sock;
extern int g_conn_buffer_size;

int g_split_buff_size = 0;

/* this needs to be reusable in other clients/libraries as part of BRAIN distrib. */
static int g_client_sock = 0;
static char *g_recv_buffer = NULL;
static int g_recv_size = 0;
static char *g_send_buffer = NULL;
static int g_send_size = 0;




void brsh_handle_reply(int in_reply_type, void *in_data, int in_size);


void client_disconnect(void)
{
    /* close connection */
    close(g_client_sock);
}


void client_connect(void)
{
    /* allocate buffers */
    g_split_buff_size = g_conn_buffer_size / 2;
    g_recv_buffer = malloc(g_split_buff_size);
    g_send_buffer = malloc(g_split_buff_size);
    
    
    /* create a socket */
    g_client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_client_sock < 0)
        fatal("Couldn't create client socket.\n");
    
    /* prepare the server address */
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    if (strlen(g_braind_server_sock) > sizeof(server_addr.sun_path)-1)
        fatal("Server name is too long.\n");
    strcpy(server_addr.sun_path, g_braind_server_sock);
    
    /* connect to the server */
    if (connect(g_client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)))
    {
        switch (errno)
        {
            case ECONNREFUSED:
                fatal("BRAIN daemon is not running.\n", errno);
            default:
                fatal("Couldn't connect to BRAIN - unknown system error %d\n", errno);
                break;
        }
    }
    
    /* make the connection socket non-blocking and asyncronous */
    if (fcntl(g_client_sock, F_SETFL, O_ASYNC | O_NONBLOCK))
        fatal("Couldn't make connection socket asyncronous.\n");
}


/* call void brsh_handle_reply(int in_reply_type, void *in_data, int in_size)
 for each reply we get from the server */
void client_poll(void)
{
    /* check for closure, or data available/writable */
    struct pollfd poll_what;
    poll_what.fd = g_client_sock;
    poll_what.events = POLLRDNORM | POLLWRNORM | POLLERR | POLLHUP;
    poll_what.revents = 0;
    if (poll(&poll_what, 1, 0) <= 0) return;
    if (poll_what.revents & (POLLERR | POLLHUP))
    {
        /* connection closed */
        fatal("Connection terminated by server.\n");
    }
    
    if (poll_what.revents & POLLRDNORM)
    {
        /* read incoming */
        
        while (g_recv_size < g_split_buff_size)
        {
            long bytes = read(g_client_sock,
                              g_recv_buffer + g_recv_size,
                              g_split_buff_size - g_recv_size);
            if (bytes <= 0) break;
            g_recv_size += bytes;
        }
        
        /* despatch requests */
        while (g_recv_size >= 3)
        {
            int req_type = g_recv_buffer[0];
            int req_size = (g_recv_buffer[1] << 8) + g_recv_buffer[2];
            if ((req_size < 0) || (req_size > g_split_buff_size))
                fatal("Connection buffer overflow.\n");
   
            int actual_size = 3 + req_size;
            if (g_recv_size < actual_size) break;
            
            brsh_handle_reply(req_type, g_recv_buffer + 3, req_size);
    
            /* TODO: this could eventually use a ring buffer to avoid
             moving memory all the time */
            memmove(g_recv_buffer,
                    g_recv_buffer + actual_size,
                    g_recv_size - actual_size);
            g_recv_size -= actual_size;
        }
    }
    if ((poll_what.revents & POLLWRNORM) && (g_send_size > 0))
    {
        /* write outgoing */
        while (g_send_size > 0)
        {
            long bytes = write(g_client_sock, g_send_buffer, g_send_size);
            if (bytes <= 0) break;
            memmove(g_send_buffer,
                    g_send_buffer + bytes,
                    g_send_size - bytes);
            g_send_size -= bytes;
        }
    }
}


void client_send_request(int in_req_type, void *in_data, int in_size)
{
    int actual_size = in_size + 3;
    if (g_split_buff_size - g_send_size < actual_size)
    {
        fatal("Couldn't send reply, buffer overflow.\n"); /* TODO: should do something else with this! */
        return;
    }
    
    g_send_buffer[g_send_size] = in_req_type;
    g_send_buffer[g_send_size + 1] = (0xFF00 & in_size) >> 8;
    g_send_buffer[g_send_size + 2] = 0xFF & in_size;
    memcpy(g_send_buffer + g_send_size + 3, in_data, in_size);
    g_send_size += actual_size;
}



/*
 write(g_client_sock, "\xA\x0\x6Hello", 9);
 write(g_client_sock, "\xF\x0\x0", 3);
 write(g_client_sock, "\xF\x0\x0", 3);
 write(g_client_sock, "\x4\x0\x4" "Bye", 7);
 char buffer[100];
 printf("reading\n");
 long bytes = read(g_client_sock, buffer, 100);
 int req_type = buffer[0];
 int req_size = (buffer[1] << 8) + buffer[2];
 printf("RPY %d %s (%ld)\n", req_type, buffer + 3, req_size);
 */

