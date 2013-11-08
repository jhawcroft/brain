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
#ifndef BRAIN_CLIENT_INT_H
#define BRAIN_CLIENT_INT_H


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>

#include "../../includes/client.h"
#include "../../includes/protocol.h"
#include "../config.h"
#include "../protocol-int.h"
#include "../mem/alloc.h"
#include "../util/util.h"
#include "../util/conf.h"
#include "../fatal.h"


struct brain_client_struct
{
    int split_buff_size;
    
    int client_sock;
    
    char *recv_buffer;
    int recv_size;
    char *send_buffer;
    int send_size;
    
    time_t last_idle;
    
    void *context;
    brain_client_message_cb message_cb;
    
    int has_comm_error;
};





#endif
