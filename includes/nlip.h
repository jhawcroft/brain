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
/* Natural language input processing plug-in API */

#ifndef BRAIN_NLIP_H
#define BRAIN_NLIP_H


typedef void* NLConceptRef;


typedef int (*NLTokenizerStartup) (void **out_context);
typedef void (*NLTokenizerShutdown) (void *in_context);
typedef int (*NLTokenizerGetNextToken) (void *in_context, char const *in_characters, int in_length,
int *out_offset, int *out_length, NLConceptRef *out_concept, char **out_value);

typedef struct NLTokenizer
{
    NLTokenizerStartup          startup;
    NLTokenizerShutdown         shutdown;
    NLTokenizerGetNextToken     get_next_token;
    void                        *context;
    
} NLTokenizer;



typedef int (*NLWordizerStartup) (void **out_context);
typedef void (*NLWordizerShutdown) (void *in_context);
typedef int (*NLWordizerIdentifyWord) (void *in_context, char const *in_characters, int in_length,
NLConceptRef *out_concept);

typedef struct NLWordizer
{
    NLWordizerStartup          startup;
    NLWordizerShutdown         shutdown;
    NLWordizerIdentifyWord     identify_word;
    void                        *context;
    
} NLWordizer;



typedef int (*NLSplitterStartup) (void **out_context);
typedef void (*NLSplitterShutdown) (void *in_context);
typedef int (*NLSplitterGetNextEndPoint) (void *in_context, char const *in_characters, int in_length,
int *out_end_offset, int *out_end_length);

typedef struct NLSplitter
{
    NLSplitterStartup           startup;
    NLSplitterShutdown          shutdown;
    NLSplitterGetNextEndPoint   get_next_end_point;
    void                        *context;
    
} NLSplitter;


/* return/error codes */
enum
{
    NL_OK = 0,
    NL_EMEM,
    NL_ESEARCH,
};


#endif
