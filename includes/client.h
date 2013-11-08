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

#ifndef BRAIN_CLIENT_H
#define BRAIN_CLIENT_H



enum
{
    BRAIN_NO_ERROR = 0,
    BRAIN_EMEMORY,
    BRAIN_ECONFIG,
    BRAIN_ENAME,
    BRAIN_EINTERNAL,
    BRAIN_ECONN,
    BRAIN_ESYS,
};


struct brain_client_struct;
typedef struct brain_client_struct brain_client_t;


typedef void (*brain_client_message_cb) (void *in_context, int in_comm, void *in_data, int in_size);


int brain_client_configure(char const *in_alt_config_file);

brain_client_t* brain_client_create(void *in_context, brain_client_message_cb in_message_cb);
void brain_client_dispose(brain_client_t *in_client);

int brain_client_connect(brain_client_t *in_client);

int brain_client_send_request(brain_client_t *in_client, int in_req_type, void *in_data, int in_size);

void brain_client_wait_for_send(brain_client_t *in_client);


int brain_client_poll_(brain_client_t *in_client);



#endif
