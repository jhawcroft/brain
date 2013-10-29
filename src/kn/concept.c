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

#include "common.h"


/* IMPORTANT NOTE: once the application has run out of memory, it is almost
 impossible for something this complex to guarantee that it can recover;
 thus it will be more important for the allocator to track how much memory has
 been used, including by collection objects, and raise a warning with a 
 significant buffer to spare.  All this error checking is probably best replaced
 by an appropriate failure mechanism, as once it's happened, it's probably 
 too late to avoid major inconsistencies. 
 
 TODO: modify the collection classes again to make them abort on out of memory,
 and use the BRAIN memory manager, preferably in a neat way (using macros.)
 */



static void rep_dispose_cont_(void *in_context, knrep_t *in_rep)
{
    /* remove from all reps here */
    PtrSet *reps = hashmap_item(g_all_representations, in_rep->text);
    if (reps)
        ptrset_remove(reps, in_rep);
    
    if (in_rep->text) brain_free_(in_rep->text);
    brain_free_(in_rep);
}


static void concept_dispose_(knconcept_t *in_concept)
{
    hashmap_remove(g_all_concepts, in_concept->name, NULL, NULL);
    
    if (in_concept->name) brain_free_(in_concept->name);
    if (in_concept->reps) array_dispose(in_concept->reps);
    if (in_concept->supers) array_dispose(in_concept->supers);
    if (in_concept->subs) array_dispose(in_concept->subs);
    brain_free_(in_concept);
}


knconcept_t* kn_concept_define(char const *in_name)
{
    /* check a concept with the same name doesn't already exist */
    if (hashmap_contains(g_all_concepts, (void*)in_name))
        return NULL;
    
    knconcept_t *concept = brain_alloc_(sizeof(knconcept_t), 0);
    memset(concept, 0, sizeof(knconcept_t));
    
    concept->name = brain_strdup(in_name);
    concept->reps = array_create(NULL, (ArrayCBDisposeItem)rep_dispose_cont_);
    concept->supers = array_create(NULL, NULL);
    concept->subs = array_create(NULL, NULL);
    
    hashmap_add(g_all_concepts, concept->name, concept);
    
    return concept;
}


knconcept_t* kn_concept_lookup(char const *in_name)
{
    return hashmap_item(g_all_concepts, (void*)in_name);
}


void kn_concept_forget(knconcept_t *in_concept)
{
    for (knconcept_t *c = array_iterate(in_concept->supers);
         (c = array_next(in_concept->supers) ); )
    {
        array_remove(c->subs, array_index_of(c->subs, in_concept), NULL);
    }
    for (knconcept_t *c = array_iterate(in_concept->subs);
         (c = array_next(in_concept->subs) ); )
    {
        array_remove(c->supers, array_index_of(c->supers, in_concept), NULL);
    }
    concept_dispose_(in_concept);
}


int kn_rep_add(knconcept_t *in_concept, char const *in_rep, knconcept_t *in_lang)
{
    knrep_t *rep = brain_alloc_(sizeof(knrep_t), 0);
    memset(rep, 0, sizeof(knrep_t));
    
    rep->text = brain_strdup(in_rep);
    rep->lang = in_lang;
    rep->owner = in_concept;
    
    array_append(in_concept->reps, rep);
    
    PtrSet *reps = hashmap_item(g_all_representations, rep->text);
    if (!reps)
    {
        reps = ptrset_create(NULL, NULL);
        hashmap_add(g_all_representations, brain_strdup(rep->text), reps);
    }
    ptrset_add(reps, rep);
    
    return KN_OK;
    
kn_rep_add_error:
    rep_dispose_cont_(NULL, rep);
    return KN_EMEM;
}


void kn_relate_isa(knconcept_t *in_sub_concept, knconcept_t *in_super_concept)
{
    array_append(in_sub_concept->supers, in_super_concept);
    array_append(in_super_concept->subs, in_sub_concept);
}


int kn_query_isa(knconcept_t *in_sub_concept, knconcept_t *in_super_concept)
{
    return (array_index_of(in_sub_concept->supers, in_super_concept) != ARRAY_INVALID_INDEX);
}


char const* kn_concept_name(knconcept_t *in_concept)
{
    return in_concept->name;
}




