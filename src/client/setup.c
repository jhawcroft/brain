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
#include "client-int.h"



int brain_client_configure(char const *in_alt_config_file)
{
    if (brain_configure_(in_alt_config_file))
        return BRAIN_ECONFIG;
    return BRAIN_NO_ERROR;
}


void brain_client_dispose(brain_client_t *in_client)
{
    if (in_client->client_sock >= 0) close(in_client->client_sock);
    if (in_client->recv_buffer) brain_free_(in_client->recv_buffer);
    if (in_client->send_buffer) brain_free_(in_client->send_buffer);
    brain_free_(in_client);
}


brain_client_t* brain_client_create(void *in_context, brain_client_message_cb in_message_cb)
{
    brain_client_t *client = brain_alloc_(sizeof(brain_client_t), 0);
    if (!client) return NULL;
    
    memset(client, 0, sizeof(brain_client_t));
    client->split_buff_size = BRAIN_BUFFER / 2;
    client->client_sock = -1;
    
    client->context = in_context;
    client->message_cb = in_message_cb;
    
    client->recv_buffer = brain_alloc_(client->split_buff_size, 0);
    if (!client->recv_buffer) goto create_failed;
    client->send_buffer = brain_alloc_(client->split_buff_size, 0);
    if (!client->recv_buffer) goto create_failed;
    
    return client;
    
create_failed:
    brain_client_dispose(client);
    return NULL;
}


int brain_client_connect(brain_client_t *in_client)
{
    in_client->client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (in_client->client_sock < 0)
        return BRAIN_EMEMORY;
    
    /* prepare the server address */
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    if (strlen(g_brain_socket_name) > sizeof(server_addr.sun_path)-1)
        return BRAIN_ENAME;
    strcpy(server_addr.sun_path, g_brain_socket_name);
    
    /* connect to the server */
    if (connect(in_client->client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)))
    {
        switch (errno)
        {
            case ECONNREFUSED:
                return BRAIN_ECONN;
            default:
                return BRAIN_ESYS;
        }
    }
    
    /* make the connection socket non-blocking and asyncronous */
    if (fcntl(in_client->client_sock, F_SETFL, O_ASYNC | O_NONBLOCK))
        return BRAIN_EINTERNAL;
    
    return BRAIN_NO_ERROR;
}





