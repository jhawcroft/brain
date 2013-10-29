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
/* development test program */

#include <stdio.h>
#include <stdlib.h>

#include "nl.h"
#include "kn.h"


void fatal(char const *in_message)
{
    printf("fatal error: %s\n", in_message);
    abort();
}


int main(int argc, const char * argv[])
{
    if (nl_startup()) fatal("couldn't start NL");
    if (kn_startup()) fatal("couldn't start KN");
    
    void test_nli_parse(void);
    test_nli_parse();
    
    return 0;
}

