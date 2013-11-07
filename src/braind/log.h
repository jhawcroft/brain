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

#ifndef BRAIND_LOG_H
#define BRAIND_LOG_H

#include <stdarg.h>


enum
{
    BRAIN_NOTICE = 0,
    BRAIN_WARNING,
    BRAIN_ERROR,
    BRAIN_DEBUG,
};


enum
{
    SYSLOG_DAEMON = -1,
    SYSLOG_LOCAL0 = 0,
    SYSLOG_LOCAL1,
    SYSLOG_LOCAL2,
    SYSLOG_LOCAL3,
    SYSLOG_LOCAL4,
    SYSLOG_LOCAL5,
    SYSLOG_LOCAL6,
    SYSLOG_LOCAL7,
};


void log_init(char const *in_log_name, int in_syslog_local);
void lprintf(int in_level, char const *in_message, ...);
void lvprintf(int in_level, char const *in_message, va_list in_args);
void log_flush(void);
void log_deinit(void);
void log_debug(void);
void log_stdout(int in_on);



#endif
