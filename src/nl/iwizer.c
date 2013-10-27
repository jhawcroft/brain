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
/* input nl word-concept lookup */

#include "common.h"

// job is to lookup words associated concepts and tag the words appropriately using a wordizer

int nl_wordize(NLInput *in_input, NLWordizer *in_wordizer)
{
    return NL_OK;
}



/* DEPRECATED? */
static void nl_lookup_concepts_(NLInput *in_input)
{
    /*for (int s = 0; s < in_input->sentence_count; s++)
     {
     NLSentence *sentence = in_input->sentences[s];
     for (int t = 0; t < sentence->token_count; t++)
     {
     NLToken *token = sentence->tokens[t];
     if (token->flags & NL_TOKEN_FLAGS_TEXT)
     token->concepts = kb_token_text_to_concepts(token->characters);
     }
     }*/
}
