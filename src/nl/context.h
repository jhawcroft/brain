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

#ifndef BRAIN_NL_CONTEXT_H
#define BRAIN_NL_CONTEXT_H


#include "common.h"
#include "parts.h"





typedef struct NLContext
{
    NLTokenizer *tokenizers;
    int tokenizer_count;
    NLSplitter *word_splitter;
    NLSplitter *sentence_splitter;
    NLWordizer *wordizer;
    
} NLContext;


extern NLContext g_nl_context;


int nl_register_tokenizer(NLTokenizer *in_tokenizer);
int nl_register_word_splitter(NLSplitter *in_splitter);
int nl_register_sentence_splitter(NLSplitter *in_splitter);
int nl_register_wordizer(NLWordizer *in_wordizer);



#endif
