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
/* UNIX domain socket, brain client library */

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

// should go in /tmp/brain/
// or /var/run/brain/

static int g_client_sock = 0;



void client_connect(char const *in_server_name)
{
    if (chdir("/Users/josh/brain/"))
    {
        printf("Failed to change directory.\n");
        exit(EXIT_FAILURE);
    }
    
    /* create a socket */
    g_client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_client_sock < 0)
    {
        fprintf(stdout, "Couldn't create client socket.\n");
        exit(EXIT_FAILURE);
    }
    
    /* prepare the server address */
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    if (strlen(in_server_name) > sizeof(server_addr.sun_path)-1)
    {
        fprintf(stdout, "Server name is too long.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(server_addr.sun_path, in_server_name);
    
    /* connect to the server */
    printf("COnnecting...\n");
    if (connect(g_client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)))
    {
        fprintf(stdout, "Couldn't connect to BRAIN - unknown system error %d\n", errno);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected.\n");
    
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
    
    /* close connection */
    close(g_client_sock);
    
    printf("Done\n");
}







