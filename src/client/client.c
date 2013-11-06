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
/* BRAIN client; connects to braind and exchanges requests & responses with it */

#include "client-int.h"



/* call void brsh_handle_reply(int in_reply_type, void *in_data, int in_size)
 for each reply we get from the server */
int brain_client_poll_(brain_client_t *in_client)
{
    if (in_client->has_comm_error) return BRAIN_ECONN;
    
    /* send IDLE to keep connection open */
    time_t now = time(NULL);
    if (now - in_client->last_idle > BRAIN_COMM_CONN_IDLE_SECS)
    {
        in_client->last_idle = now;
        brain_client_send_request(in_client, BRAIN_COMM_IDLE, NULL, 0);
    }
    
    /* check for closure, or data available/writable */
    struct pollfd poll_what;
    poll_what.fd = in_client->client_sock;
    poll_what.events = POLLRDNORM | POLLWRNORM | POLLERR | POLLHUP;
    poll_what.revents = 0;
    if (poll(&poll_what, 1, 0) <= 0) return BRAIN_NO_ERROR;
    if (poll_what.revents & (POLLERR | POLLHUP))
    {
        /* connection closed */
        in_client->has_comm_error = 1;
        return BRAIN_ECONN;
    }
    
    if (poll_what.revents & POLLRDNORM)
    {
        /* read incoming */
        while (in_client->recv_size < in_client->split_buff_size)
        {
            long bytes = read(in_client->client_sock,
                              in_client->recv_buffer + in_client->recv_size,
                              in_client->split_buff_size - in_client->recv_size);
            if (bytes <= 0) break;
            in_client->recv_size += bytes;
        }
        
        /* despatch requests */
        while (in_client->recv_size >= 3)
        {
            int req_type = in_client->recv_buffer[0];
            int req_size = (in_client->recv_buffer[1] << 8) + in_client->recv_buffer[2];
            if ((req_size < 0) || (req_size > in_client->split_buff_size))
            {
                /* connection buffer overflow */
                in_client->has_comm_error = 1;
                return BRAIN_EMEMORY;
            }
   
            int actual_size = 3 + req_size;
            if (in_client->recv_size < actual_size) break;
            
            if (in_client->message_cb)
                in_client->message_cb(in_client->context, req_type, in_client->recv_buffer + 3, req_size);
    
            /* TODO: this could eventually use a ring buffer to avoid
             moving memory all the time */
            memmove(in_client->recv_buffer,
                    in_client->recv_buffer + actual_size,
                    in_client->recv_size - actual_size);
            in_client->recv_size -= actual_size;
        }
    }
    if ((poll_what.revents & POLLWRNORM) && (in_client->send_size > 0))
    {
        /* write outgoing */
        while (in_client->send_size > 0)
        {
            long bytes = write(in_client->client_sock, in_client->send_buffer, in_client->send_size);
            if (bytes <= 0) break;
            memmove(in_client->send_buffer,
                    in_client->send_buffer + bytes,
                    in_client->send_size - bytes);
            in_client->send_size -= bytes;
        }
    }
    
    return BRAIN_NO_ERROR;
}


int brain_client_send_request(brain_client_t *in_client, int in_req_type, void *in_data, int in_size)
{
    int actual_size = in_size + 3;
    if (in_client->split_buff_size - in_client->send_size < actual_size)
        return BRAIN_EMEMORY;
    
    in_client->send_buffer[in_client->send_size] = in_req_type;
    in_client->send_buffer[in_client->send_size + 1] = (0xFF00 & in_size) >> 8;
    in_client->send_buffer[in_client->send_size + 2] = 0xFF & in_size;
    memcpy(in_client->send_buffer + in_client->send_size + 3, in_data, in_size);
    in_client->send_size += actual_size;
    
    return BRAIN_NO_ERROR;
}



#define WAIT_USECS 5000

void brain_client_wait_for_send(brain_client_t *in_client)
{
    while (in_client->send_size > 0)
    {
        if (brain_client_poll_(in_client)) break;
        usleep(WAIT_USECS);
    }
}


