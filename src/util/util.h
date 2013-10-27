//
//  util.h
//  PrototypeNLUnit
//
//  Created by Joshua Hawcroft on 13/10/13.
//  Copyright (c) 2013 Joshua Hawcroft. All rights reserved.
//

#ifndef PrototypeNLUnit_util_h
#define PrototypeNLUnit_util_h

char* jh_strdup(char const *in_string);
char* strdup_to_upper(char const *in_string);


#define MAX_INT_PTR 1024
extern char int_ptr_vals[MAX_INT_PTR + 1];

#define INT_TO_PTR(inInt) ( ((inInt < 0) || (inInt > MAX_INT_PTR)) ? NULL : int_ptr_vals + inInt )
#define PTR_TO_INT(inPtr) ( (!inPtr) ? 0 : inPtr - int_ptr_vals )

#endif
