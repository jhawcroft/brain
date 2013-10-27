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

#ifndef BRAIN_NL_INPUT_H
#define BRAIN_NL_INPUT_H


#include "common.h"


typedef struct NLInput
{
    Array *sentences; /* NLSentence */
    
} NLInput;



int nl_tokenize(NLInput *in_input, NLTokenizer *in_tokenizer);
int nl_split_sentences(NLInput *in_input, NLSplitter *in_splitter);
int nl_split_words(NLInput *in_input, NLSplitter *in_splitter);
int nl_wordize(NLInput *in_input, NLWordizer *in_wordizer);
int nl_trim_empty_sentences(NLInput *in_input);


int nl_input_parse(char const *in_utterance, NLInput **out_result);
void nl_input_dispose(NLInput *in_input);


#if DEBUG == 1
void nl_input_debug(NLInput *in_input);
#endif



#endif
