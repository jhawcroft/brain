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

#ifndef BRAIND_SERVER_H
#define BRAIND_SERVER_H


#include <time.h>


typedef struct connection_struct
{
    int sock;
    time_t last_activity;
    int z_count;
    char *recv_buffer;
    int recv_size;
    char *writ_buffer;
    int writ_size;
    
} connection_t;


void server_respond(connection_t *in_conn, int in_type, void *in_data, int in_size);
void despatch_request(connection_t *in_conn, int in_type, void *in_data, int in_size);


#endif
