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
#include <stdarg.h>
#include <stdlib.h>

#include "ui.h"


void fatal(char const *in_message, ...)
{
    va_list arg_list;
    va_start(arg_list, in_message);
    vprintf(in_message, arg_list);
    va_end(arg_list);
    if (brsh_ui_inited()) brsh_shutdown(EXIT_FAILURE);
    exit(EXIT_FAILURE);
}



