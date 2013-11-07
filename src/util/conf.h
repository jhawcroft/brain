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

#ifndef BRAIN_CONF_H
#define BRAIN_CONF_H


int brain_configure_(char const *in_alternate_config_file);


extern char const *const g_brain_conf_name;
extern char const *const g_brain_thoughts;
extern char const *const g_brain_bin;
extern char const *const g_brain_socket_name;
extern char const *const g_brain_log_name;

extern int const g_brain_buffer;



#endif
