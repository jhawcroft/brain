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
/*
 Program interface for the natural language processing functionality 
 */

#ifndef BRAIN_NL_H
#define BRAIN_NL_H


#include "../../includes/nlip.h"


/* initalize the natural language processing system */
int nl_startup(void);


/* represents the result of processing natural language input (eg. English) */
typedef struct nlmeaning_struct
{
    char *meaning;
    char **arguments;
    int argument_count;
    
} nlmeaning_t;


/* converts an utterance (1 or more sentences) into a meaning (0 or more meanings) */
int nl_input_to_meanings(char const *in_utterance, nlmeaning_t **out_meanings[], int *out_count);

/* serializes an array of nlmeaning_t to a human-readable string representation */
char* nl_meanings_to_string(nlmeaning_t *in_meanings, int in_count, int in_options);

/* unserializes a human-readable string representation of a meaning into an array of nlmeaning_t */
int nl_string_to_meanings(char const *in_string, nlmeaning_t **out_meanings, int *out_count, int in_hints);

/* converts an array of nlmeaning_t to a natural language utterance (1 or more sentences) */
int nl_meanings_to_output(nlmeaning_t *in_meanings, int in_count, char **out_output, int in_hints);


/* cleanup after yourself! */
void nl_meanings_dispose(nlmeaning_t *in_meanings, int in_count);
void nl_string_dispose(char *in_string);

int nl_shutdown(void);



#endif
