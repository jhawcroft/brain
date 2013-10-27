/*
 "IFSAI" - Index For Sorted Array Item
 v. 1.0
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 (See the header file ifsai.h for a description and configuration)
 
 ----------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ifsai.h"


IFSAI_INDEX_TYPE ifsai(void *in_list, void *in_item,
                       IFSAI_INDEX_TYPE (*in_func_count) (void *in_list),
                       int (*in_func_compare) (void *in_list, void *in_left, void *in_right),
                       void* (*in_func_get) (void *in_list, IFSAI_INDEX_TYPE in_index))
{
    IFSAI_INDEX_TYPE count = in_func_count(in_list);
    if (count == 0) return 0;
    IFSAI_INDEX_TYPE range_start = 0;
    IFSAI_INDEX_TYPE range_end = count - 1;
    while (range_end - range_start > 0)
    {
        IFSAI_INDEX_TYPE range_middle = (range_end - range_start) / 2 + range_start;
        void *middle_item = in_func_get(in_list, range_middle);
        int comp = in_func_compare(in_list, in_item, middle_item);
        if (comp < 0) range_end = range_middle;
        else if (comp == 0) return range_middle;
        else range_start = range_middle + 1;
    }
    int comp = in_func_compare(in_list, in_func_get(in_list, range_start), in_item);
    if (comp < 0) return range_start + 1;
    else return range_start;
}



/* Tests */

#if COMPILE_IFSAI_TEST_ == 1

#include <stdio.h>
#include <assert.h>


static int test_list_a[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
static int test_list_b[10] = { 2, 3, 4, 5, 5, 8, 10, 11, 12, 13 };
static int *test_list_c = NULL;
static int test_list_d[1] = { 7 };
static int test_list_e[2] = { 5, 6 };
static int test_list_f[3] = { 5, 6, 8 };
static int test_list_g[4] = { 5, 6, 7, 8 };
static int test_list_h[2] = { 5, 7 };

static int test_zero = 0;
static int *test_item_0 = &test_zero;

static int *test_item_1 = &(test_list_a[0]);
static int *test_item_2 = &(test_list_a[1]);
static int *test_item_3 = &(test_list_a[2]);
static int *test_item_4 = &(test_list_a[3]);
static int *test_item_5 = &(test_list_a[4]);
static int *test_item_6 = &(test_list_a[5]);
static int *test_item_7 = &(test_list_a[6]);
static int *test_item_8 = &(test_list_a[7]);
static int *test_item_9 = &(test_list_a[8]);
static int *test_item_10 = &(test_list_a[9]);

static int *test_item_11 = &(test_list_b[7]);
static int *test_item_12 = &(test_list_b[8]);
static int *test_item_13 = &(test_list_b[9]);


static IFSAI_INDEX_TYPE test_count(void *in_list)
{
    if (in_list == test_list_a) return 10;
    else if (in_list == test_list_b) return 10;
    else if (in_list == test_list_c) return 0;
    else if (in_list == test_list_d) return 1;
    else if (in_list == test_list_e) return 2;
    else if (in_list == test_list_f) return 3;
    else if (in_list == test_list_g) return 4;
    else if (in_list == test_list_h) return 2;
    assert(0);
}


static int test_compare(void *in_list, void *in_left, void *in_right)
{
    int *left = (int*)in_left;
    int *right = (int*)in_right;
    if (*left < *right) return -1;
    else if (*left == *right) return 0;
    else return 1;
}


static void* test_get(void *in_list, IFSAI_INDEX_TYPE in_index)
{
    return &(((int*)in_list)[in_index]);
}


void ifsai_test(void)
{
    printf("ifsai: running tests...");
    
    assert( ifsai(test_list_a, test_item_1, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_a, test_item_2, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_a, test_item_3, &test_count, &test_compare, &test_get) == 2 );
    assert( ifsai(test_list_a, test_item_4, &test_count, &test_compare, &test_get) == 3 );
    assert( ifsai(test_list_a, test_item_5, &test_count, &test_compare, &test_get) == 4 );
    assert( ifsai(test_list_a, test_item_6, &test_count, &test_compare, &test_get) == 5 );
    assert( ifsai(test_list_a, test_item_7, &test_count, &test_compare, &test_get) == 6 );
    assert( ifsai(test_list_a, test_item_8, &test_count, &test_compare, &test_get) == 7 );
    assert( ifsai(test_list_a, test_item_9, &test_count, &test_compare, &test_get) == 8 );
    assert( ifsai(test_list_a, test_item_10, &test_count, &test_compare, &test_get) == 9 );
    
    assert( ifsai(test_list_a, test_item_11, &test_count, &test_compare, &test_get) == 10 );
    assert( ifsai(test_list_a, test_item_12, &test_count, &test_compare, &test_get) == 10 );
    assert( ifsai(test_list_a, test_item_13, &test_count, &test_compare, &test_get) == 10 );
    assert( ifsai(test_list_a, test_item_0, &test_count, &test_compare, &test_get) == 0 );
    
    assert( ifsai(test_list_b, test_item_6, &test_count, &test_compare, &test_get) == 5 );
    assert( ifsai(test_list_b, test_item_5, &test_count, &test_compare, &test_get) == 4 );
    assert( ifsai(test_list_b, test_item_7, &test_count, &test_compare, &test_get) == 5 );
    assert( ifsai(test_list_b, test_item_9, &test_count, &test_compare, &test_get) == 6 );
    assert( ifsai(test_list_b, test_item_10, &test_count, &test_compare, &test_get) == 6 );
    assert( ifsai(test_list_b, test_item_3, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_b, test_item_2, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_b, test_item_1, &test_count, &test_compare, &test_get) == 0 );
    
    assert( ifsai(test_list_c, test_item_1, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_c, test_item_2, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_c, test_item_3, &test_count, &test_compare, &test_get) == 0 );
    
    assert( ifsai(test_list_d, test_item_6, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_d, test_item_7, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_d, test_item_8, &test_count, &test_compare, &test_get) == 1 );
    
    assert( ifsai(test_list_e, test_item_4, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_e, test_item_5, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_e, test_item_6, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_e, test_item_7, &test_count, &test_compare, &test_get) == 2 );
    
    assert( ifsai(test_list_f, test_item_4, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_f, test_item_5, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_f, test_item_6, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_f, test_item_7, &test_count, &test_compare, &test_get) == 2 );
    assert( ifsai(test_list_f, test_item_8, &test_count, &test_compare, &test_get) == 2 );
    assert( ifsai(test_list_f, test_item_9, &test_count, &test_compare, &test_get) == 3 );
    
    assert( ifsai(test_list_g, test_item_4, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_g, test_item_5, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_g, test_item_6, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_g, test_item_7, &test_count, &test_compare, &test_get) == 2 );
    assert( ifsai(test_list_g, test_item_8, &test_count, &test_compare, &test_get) == 3 );
    assert( ifsai(test_list_g, test_item_9, &test_count, &test_compare, &test_get) == 4 );
    
    assert( ifsai(test_list_h, test_item_4, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_h, test_item_5, &test_count, &test_compare, &test_get) == 0 );
    assert( ifsai(test_list_h, test_item_6, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_h, test_item_7, &test_count, &test_compare, &test_get) == 1 );
    assert( ifsai(test_list_h, test_item_8, &test_count, &test_compare, &test_get) == 2 );
    
    printf("ok.\n");
}


int main(int argc, const char * argv[])
{
    ifsai_test();
    return 0;
}


#endif

