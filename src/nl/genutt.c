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
 Natural Language Meaning => Sentence Generation
 */



/* can also have 'fillers' for between concept queries
 used only for output generation;
 these are potentially OPTIONAL concept queries,
 where they're not used for the primary matching, but may match if present,
 and fillers that don't match ever, but just get output during output generation
 to help make complete sentences */

/*
 
 *** NOT PRESENTLY COMPILED **** PUT BACK INTO PROJECT ONCE WORKING ON THIS BIT
 
 */

#include "common.h"


typedef struct NLOutputSentence_
{
    NLPattern *pattern; /* purely for inspection purposes when deciding if to combine/eliminate certain sentences;
                         or how to mutate sentences to avoid sounding like a robot */
    Array *concept_sequence;
    
} NLOutputSentence;


// lookup the meaning's meaning maps in the hash table g_all_meaning_maps in kb
// look for those that have the same number of arguments
// choose one (at random?)

// query concept list of a pattern could include suggested joining words/concepts between query concepts
// which would be ignored during input processing
// or optional concept queries, that are filled at the end of a successful pattern match on input,
// and thus constitute arguments in the map,
// and can be remapped on output also (but explcitly and based on how verbose the output is intended to be)
// also, adjectives can be chosen based on various criteria associated with the feeling, urgency, accuracy
// of the meaning and context (to be added to NL meaning)

// and the knowledge net, which will associate similar adjectives
// as well as having multiple representations available for a given concept/adjective

// and we can select one for the appropriate language (add to meaning/ 'conversational context' new struct?)
// and we can apply in the parser hints to the representations (such as? eg. don't use this rep for gen)

// another possibility is to include additional output specific forms in a pattern
// so you're urged to think about the inverse of a statement, and statements are still logically grouped


static int meaning_has_named_arg(NLMeaning *in_meaning, char const *in_name)
{
    size_t arg_count = array_count(in_meaning->arguments);
    for (size_t i = 0; i < arg_count; i += 2)
    {
        char const *arg_name = array_item(in_meaning->arguments, i);
        if (strcmp(arg_name, in_name) == 0) return 1;
    }
    return 0;
}



size_t meaning_map_references_query_(NLPatternMeaningMap *in_meaning_map, NLPatternConceptQuery *in_query)
{
    size_t mm_qi_count = array_count(in_meaning_map->qualifier_query_indicies) / 3;
    for (size_t mm_qi = 0; mm_qi < mm_qi_count; mm_qi++)
    {
        if (array_item(in_meaning_map->qualifier_query_indicies, mm_qi * 3 + 1) == &PMM_QUALIFIER_MATCHED)
        {
            char const *pattern_token_name = array_item(in_meaning_map->qualifier_query_indicies, mm_qi * 3 + 2);
            if (strcmp(pattern_token_name, in_query->programming_name) == 0)
                return mm_qi;
        }
    }
    return ARRAY_INVALID_INDEX;
}




NLOutputSentence* nl_meaning_to_sentence(NLMeaning *in_meaning)
{
    NLOutputSentence *output;
    NLPattern *pattern;
    NLPatternMeaningMap *meaning_map;
    Array *meaning_maps = kb_meaning_maps_for_meaning(in_meaning->meaning);
    
    if (!meaning_maps) return NULL;
    
    long m_arg_count = array_count(in_meaning->arguments) / 2;
    
    int mm_count = (int)array_count(meaning_maps);
    for (int mm = 0; mm < mm_count; mm++)
    {
        meaning_map = array_item(meaning_maps, mm);
        
        /* check if we have same number of arguments */
        if (array_count(meaning_map->qualifier_query_indicies) / 3 != m_arg_count) continue;
        
        /* check argument names;
         sequence probably does matter at the moment
         all of this is a bit in need of documentation .....  */
        pattern = (NLPattern*)meaning_map->referencing_pattern;
        
        
        /* triples: name, type, value (depends on type) */
        // need to actually review these i can't remmeber what they are or how it works..
        
        size_t mm_qi_count = array_count(meaning_map->qualifier_query_indicies) / 3;
        for (size_t mm_qi = 0; mm_qi < mm_qi_count; mm_qi++)
        {
            Concept *meaning_arg_concept = array_item(in_meaning->arguments, mm_qi * 2 + 1);
            Concept *meaning_map_arg_concept = NULL;
            if (array_item(meaning_map->qualifier_query_indicies, mm_qi * 3 + 1) == &PMM_QUALIFIER_CONCEPT)
            {
                /* meaning map has literal concept */
                meaning_map_arg_concept = array_item(meaning_map->qualifier_query_indicies, mm_qi * 3 + 2);
            }
            else
            {
                /* meaning map has concept query name */
                char const *pattern_token_name = array_item(meaning_map->qualifier_query_indicies, mm_qi * 3 + 2);
                for (NLPatternConceptQuery *query = array_iterate(pattern->concept_queries);
                     (query = array_next(pattern->concept_queries)); )
                {
                    if (strcmp(pattern_token_name, query->programming_name) == 0)
                    {
                        meaning_map_arg_concept = query->concept;
                        break;
                    }
                }
            }
            
            if (!kb_query_concept_isa_concept(meaning_arg_concept, meaning_map_arg_concept))
                goto skip_meaning_map;
        }
        
        /* found a matching meaning map */
        goto matched_meaning_map;
        
    skip_meaning_map:
        continue;
    }
    
    /* no matching meaning map;
     I guess eventually we should have a way to find similar meanings
     so that we can try to 'speak',
     right now I'm not sure what will happen */
    return NULL;
    
matched_meaning_map:
    /* got pattern (NLPattern*) and meaning map (NLMeaningMap*) */
    
    output = calloc(1, sizeof(NLOutputSentence));
    output->concept_sequence = array_create();
    output->pattern = pattern;
    
    for (NLPatternConceptQuery *query = array_iterate(pattern->concept_queries);
         (query = array_next(pattern->concept_queries)); )
    {
        
        //need to scan the meaning map here for references to this query
        // if no reference, just append the concept
        
        size_t argument_index = meaning_map_references_query_(meaning_map, query);
        if (argument_index == ARRAY_INVALID_INDEX)
            array_append(output->concept_sequence, query->concept);
        
        else
            array_append(output->concept_sequence, array_item(in_meaning->arguments, argument_index * 2 + 1));
        
        // if there is a reference, use the matching meaning map argument index
        // to index the input meaning and use the supplied concept instead
        
        
    }
    
    return output;
}

/*
 
 // where the concept is flagged as adjective, have a way to find very similar and less similar,
 // and determine if to substitute based on precision required when it's an argument being provided (some are as-is)
 
 // have concepts flagged as more precise or less precise in kb - and likewise for various representations
 
 // ideally the representations themselves are all fairly accurate for a concept, with few exceptions
 // relations should be between concepts, not over complexificating representations!
 
 // even having no controls on representaions for output and makign it all go via concepts would make more sense
 
 // doesn't use (non argument based) subclasses, only similar concepts, not subconcepts
 // for argument based output, use accuracy of meaning to determine how closely to stick to that which
 // was supplied, can go up one level possibly - become more general ie. super concept (unlike with input)
 
 */


char const* concept_make_representation_(Concept *in_concept)
{
    for (NLRepresentation *rep = array_iterate(in_concept->representations);
         (rep = array_next(in_concept->representations)); )
    {
        return rep->normalized_text;
    }
    return NULL;
}


char* nl_meanings_to_output(Array *in_meanings)
{
    //return NULL;
    //Array *output = array_create();
    Sbuff *result = sbuff_create(SBUFF_DEFAULT_CAPACITY);
    
    int m_count = (int)array_count(in_meanings);
    for (int m = 0; m < m_count; m++)
    {
        NLMeaning *meaning = array_item(in_meanings, m);
        
        NLOutputSentence *sentence = nl_meaning_to_sentence(meaning);
        
        size_t concept_count = array_count(sentence->concept_sequence);
        for (size_t c = 0; c < concept_count; c++)
        {
            Concept *concept = array_item(sentence->concept_sequence, c);
            sbuff_append_cstring(result, concept_make_representation_(concept), SBUFF_AUTO_LENGTH);
            if (c + 1 < concept_count)
                sbuff_append_cstring(result, " ", 1);
            else
            {
                sbuff_append_cstring(result, ". ", 1);
            }
        }
        
        //array_append(output, sentence);
    }
    
    return sbuff_cstring_disposing(result);
}

/* initially focus on nl_meanings_to_output(), since it doesn't require parsing of strings
 and encompases the actual functionality to be implemented */


/*
 
 Intended use of meaning argument names:  *** TO FIX ***
 
 meanings had additional attributes defined - the idea is that it's easier for a human to remember a name
 than an index.  so the meaning concept itself should actually have the option of specifying named arguments.
 
 in the event they're present, creating a meaning from Input should use them - otherwise it shouldn't.
 the names in the meaning mapping are actually references to the meaning's names - thus if it doesn't have
 a specific name it's an error, no meaning can be synthesised.
 
 
 */


NLMeaning* string_to_meaning(char const *in_string, char const **out_tail)
{
    while (isspace(in_string[0])) in_string++;
    if (!in_string[0]) return NULL;
    
    char const *string_end = in_string;
    while ((string_end[0] != 0) && (string_end[0] != '(') && (!isspace(string_end[0]))) string_end++;
    
    size_t name_length = string_end - in_string;
    if ((name_length < 1) || (name_length > MAX_PROGRAMMING_NAME_BYTES)) return NULL;
    char name[MAX_PROGRAMMING_NAME_BYTES + 1];
    memcpy(name, in_string, name_length);
    name[name_length] = 0;
    
    in_string = string_end;
    while ((in_string[0] != 0) && (in_string[0] != '(')) in_string++;
    if (in_string[0] == 0) return NULL;
    in_string++;
    
    NLMeaning *meaning = calloc(1, sizeof(NLMeaning));
    if (!meaning) return NULL;
    meaning->meaning = kb_programming_name_concept(name);
    if (!meaning->meaning) goto string_to_meaning_error;
    meaning->arguments = array_create();
    if (!meaning->arguments) goto string_to_meaning_error;
    
    for (;;)
    {
        while (isspace(in_string[0])) in_string++;
        if (in_string[0] == ')')
        {
            in_string++;
            break;
        }
        
        string_end = in_string;
        while ( (string_end[0] != 0) && (string_end[0] != '=') && (string_end[0] != ',')
               && (string_end[0] != ')') && (!isspace(string_end[0])) ) string_end++;
        if (string_end[0] == 0) break;
        
        char const *test = string_end;
        while ((test[0] != 0) && isspace(test[0])) test++;
        
        if (test[0] == '=')
        {
            name_length = string_end - in_string;
            if (name_length < 1) goto string_to_meaning_error;
            memcpy(name, in_string, name_length);
            name[name_length] = 0;
            array_append(meaning->arguments, strdup(name));
            
            in_string = test + 1;
            while ((in_string[0] != 0) && isspace(in_string[0])) in_string++;
            
            string_end = in_string;
            while ( (string_end[0] != 0) && (string_end[0] != ',') &&
                   (string_end[0] != ')') && (!isspace(string_end[0])) ) string_end++;
            
            test = string_end;
            while ((test[0] != 0) && isspace(test[0])) test++;
        }
        // else append empty / NULL string
        
        if ((test[0] == ',') || (test[0] == ')'))
        {
            name_length = string_end - in_string;
            if (name_length < 1) goto string_to_meaning_error;
            memcpy(name, in_string, name_length);
            name[name_length] = 0;
            array_append(meaning->arguments, kb_programming_name_concept(name));
            
            in_string = test + 1;
            if (test[0] == ')') break;
            continue;
        }
        
        goto string_to_meaning_error;
    }
    
    while (isspace(in_string[0]) && (in_string[0] != 10) && (in_string[0] != 13)) in_string++;
    if ((in_string[0] != 0) && (in_string[0] != 10) && (in_string[0] != 13)) goto string_to_meaning_error;
    
    while (isspace(in_string[0])) in_string++;
    *out_tail = in_string;
    
    return meaning;
string_to_meaning_error:
    //*** need to cleanup here
    free(meaning);
    return NULL;
}


Array* nl_string_to_meanings(char const *in_string, int in_options)
{
    Array *meanings = array_create();
    
    while (in_string[0])
    {
        NLMeaning *meaning = string_to_meaning(in_string, &in_string);
        if (!meaning) break;
        array_append(meanings, meaning);
    }
    
    return meanings;
}

/*
 
 One thing I could probably do, which needn't compromise the design signficantly, in order to make the
 outputs more original, creative or interesting, would be to provide a bunch of additional (dictionary?)
 attributes on meanings (both in and out) to represent things like emotion, accuracy, timing, etc.
 
 On outputs some of these attributes could suggest a more jovial or relaxed attitude.
 
 There could also exist a complimentary plug-in mechnanism on the output side of processing,
 just prior to word selection (ie. while sentences are still separate and in concept form) using the 
 same UNIX scripting capability that is planned for the core of the BRAIN project and/or the NL C shared
 object modules.
 
 This module could read those attributes and easily lookup sentence starters, tails and other augmenting
 stuff that could be tacked on to the output.
 
 A filter section could be provided purely for chatbot type competitions, where-in the output has 'typos'
 introduced, timing changes and other natural-appearing character anomolies.
 
 
 
 
 */





