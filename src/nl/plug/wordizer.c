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
/* looks up a word to identify the matching concept(s) in the knowledge base */

#include "../../../includes/nlip.h"

#include <stdio.h>


static int plug_nl_wordizer_startup(void **out_context)
{
    return NL_OK;
}


static void plug_nl_wordizer_shutdown(void *in_context)
{
}


static int plug_nl_wordizer_identify_word(void *in_context, char const *in_characters, int in_length,
                                   NLConceptRef *out_concept)
{
    return NL_ESEARCH;
}


static NLWordizer _wordizer =
{
    plug_nl_wordizer_startup,
    plug_nl_wordizer_shutdown,
    plug_nl_wordizer_identify_word,
};


void nli_bi_plug_wordizer_load(void)
{
    int nl_register_wordizer(NLWordizer *in_wordizer);
    nl_register_wordizer(&_wordizer);
}


