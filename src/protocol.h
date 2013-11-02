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
/* shared communications protocol constants */

#ifndef BRAIN_COMMS_PROTOCOL_H
#define BRAIN_COMMS_PROTOCOL_H


#define BRAIN_COMM_CONN_TIMEOUT_SECS 15
#define BRAIN_COMM_CONN_IDLE_SECS 10


enum
{
    BRAIN_COMM_IDLE = 0,
    BRAIN_COMM_HELO,
    BRAIN_COMM_TEXT,
};



#endif



