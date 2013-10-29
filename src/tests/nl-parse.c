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
/* natrual language parser tests */

#include "../nl/common.h"

#include <stdio.h>
#include <stdlib.h>


#if BRAIN_COMPILE_TESTS == 1


static void fail_test(char const *in_msg)
{
    printf("failed test: %s\n", in_msg);
    abort();
}


void test_nli_parse(void)
{
    NLInput *input;
    int err;
    
    /*err = nl_input_parse("Hi Josh, how are you today?", &input);
    if (err != NL_OK) fail_test("nli_parse: 1");
    nl_input_dispose(input);
    */
    err = nl_input_parse("what's the 3 square-root of 19?", &input);
    if (err != NL_OK) fail_test("nli_parse: 1");
    nl_input_dispose(input);
    
}



#endif

