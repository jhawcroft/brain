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
/* builtin NLIP plugins; loader */

#include "../common.h"


void nli_bi_plug_acronym_load(void);
void nli_bi_plug_emoticon_load(void);

void nli_bi_plug_wordizer_load(void);

void nli_bi_plug_ss_load(void);
void nli_bi_plug_ws_load(void);

void nli_bi_plug_datetime_load(void);
void nli_bi_plug_numbers_load(void);


void nli_load_bi_plugs_(void)
{
    nli_bi_plug_acronym_load();
    nli_bi_plug_emoticon_load();
    nli_bi_plug_datetime_load();
    nli_bi_plug_numbers_load();
    
    nli_bi_plug_wordizer_load();
    
    nli_bi_plug_ss_load();
    nli_bi_plug_ws_load();
}



