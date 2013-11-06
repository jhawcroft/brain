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

#include <string.h>


#include "../../includes/client.h"
#include "event.h"
#include "ui.h"
#include "../fatal.h"


extern brain_client_t *g_client;



/* call void client_send_request(int in_req_type, void *in_data, int in_size)
 for each request we want to make of the server */

void brsh_handle_input(char const *in_input)
{
    if ((strcmp(in_input, "quit") == 0) || (strcmp(in_input, "exit") == 0))
    {
        brsh_shutdown(EXIT_SUCCESS);
    }
    brsh_printf("User: %s\n", in_input);
    
    brain_client_send_request(g_client, BRAIN_COMM_TEXT, (void*)in_input, (int) strlen(in_input) + 1);
}


void brsh_handle_reply(int in_reply_type, void *in_data, int in_size)
{
    if (in_reply_type == BRAIN_COMM_IDLE) return;
    
    switch (in_reply_type)
    {
        case BRAIN_COMM_TEXT:
            brsh_printf("Brain: %s\n", in_data);
            break;
        case BRAIN_COMM_ERRR:
            brsh_print("Brain encountered an error processing your request.\n");
            break;
        case BRAIN_COMM_HELO:
            brsh_printf("%s\n", in_data);
            break;
    }
    
}


void brsh_handle_io(void)
{
    brain_client_poll_(g_client);
}




