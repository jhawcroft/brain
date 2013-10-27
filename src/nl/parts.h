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

#ifndef BRAIN_NL_PARTS_H
#define BRAIN_NL_PARTS_H


#include "common.h"


enum
{
    NL_VAL_STRING,
};


typedef struct nlvalue_struct
{
    int type;
    union
    {
        char *string;
        
    } data;
    
} nlvalue_t;



#define NL_TOKFLG_TEXT 1


typedef struct NLToken
{
    int flags;
    char *characters;
    int length;
    PtrSet *concepts; /* NLConceptRef */
    nlvalue_t value;
    
} NLToken;


typedef struct NLSentence
{
    NLToken **tokens;
    int token_count;
    Hashmap *attributes; /* char* => nlvalue_t* */
    
} NLSentence;




NLSentence* nl_sentence_create(void);
void nl_sentence_dispose(NLSentence *in_sentence);
void nl_sentence_disposer_(void *in_context, void *in_item);


NLToken* nl_sentence_insert_token(NLSentence *in_sentence, int in_at_index, NLToken *in_token);
NLToken* nl_sentence_append_token(NLSentence *in_sentence, NLToken *in_token);
NLToken* nl_sentence_remove_token(NLSentence *in_sentence, int in_index);




NLToken* nl_token_create(char const *in_string, int in_length, int in_flags,
                         NLConceptRef in_concept, char const *in_value);
void nl_token_dispose(NLToken *in_token);







#endif
