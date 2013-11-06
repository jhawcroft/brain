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
/* Quick configuration file scanner */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>


#define MAX_CONFIG_LINE_SIZE 4096


static void mark_line_end(char *in_line)
{
    while (*in_line)
    {
        if ((*in_line == '#') || (*in_line == '\r') || (*in_line == '\n'))
        {
            *in_line = 0;
            return;
        }
        in_line++;
    }
}


static bool line_is_empty(char const *in_line)
{
    while (*in_line)
    {
        if (!isspace(*in_line)) return false;
        in_line++;
    }
    return true;
}


static char* skip_leading(char *in_line)
{
    while (in_line[0] && isspace(in_line[0])) in_line++;
    return in_line;
}


static char* skip_chars(char *in_line)
{
    while (in_line[0] && (!isspace(in_line[0]))) in_line++;
    return in_line;
}


static void trim_line_end(char *in_line)
{
    char *line_end = in_line;
    while (*line_end) line_end++;
    while ((line_end-1 > in_line) && isspace(*(line_end-1)))
        line_end--;
    *line_end = 0;
}


int brain_confscan_(char const *in_pathname,
                    int (*in_line_handler)(long in_line_number, char const *in_key, char const *in_value))
{
    int err = 0;
    long line_number = 0;
    
    FILE *fp = fopen(in_pathname, "r");
    if (!fp) return errno;
    
    static char line_buffer[MAX_CONFIG_LINE_SIZE];
    while ( fgets(line_buffer, MAX_CONFIG_LINE_SIZE, fp) )
    {
        line_number++;
        mark_line_end(line_buffer);
        if (line_is_empty(line_buffer)) continue;
        
        trim_line_end(line_buffer);
        
        char *name = skip_leading(line_buffer);
        char *name_end = skip_chars(name);
        char *value = skip_leading(name_end);
        *name_end = 0;
        
        err = in_line_handler(line_number, name, value);
        if (err) break;
    }
    
    fclose(fp);
    return err;
}


