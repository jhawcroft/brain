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

#include "error.h"



static int g_client_sock = 0;



void client_disconnect(void)
{
    /* close connection */
    close(g_client_sock);
}


void client_connect(char const *in_server_name)
{
    /* create a socket */
    g_client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_client_sock < 0)
        fatal("Couldn't create client socket.\n");
    
    /* prepare the server address */
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    if (strlen(in_server_name) > sizeof(server_addr.sun_path)-1)
        fatal("Server name is too long.\n");
    strcpy(server_addr.sun_path, in_server_name);
    
    /* connect to the server */
    if (connect(g_client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)))
        fatal("Couldn't connect to BRAIN - unknown system error %d\n", errno);
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





