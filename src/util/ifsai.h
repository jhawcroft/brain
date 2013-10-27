/*
 "IFSAI" - Index For Sorted Array Item
 v. 1.0
 Copyright 2013 Joshua Hawcroft <http://www.joshhawcroft.com>
 
 ----------------------------------------------------------------------
 Provides a handy reusable binary search for sorted C arrays.
 
 Unlike bsearch(), ifsai() always returns an index regardless of
 whether the specified item is found:
 
 -  If the item is found, you MUST still explicitly check if the item
 at the specified index is the one for which you are searching.
 
 -  If the item isn't found, you can insert it into the list at the
 index returned by ifsai().
 
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

#ifndef JoshHashMap_ifsai_h
#define JoshHashMap_ifsai_h

/*
 Compile-time Configuration
 */

/* determine the type ifsai() uses to represent indicies and counts */
#define IFSAI_INDEX_TYPE unsigned long


/*
 API
 */

/* 
 ifsai():
 -  in_list         the list you want to search
 -  in_item         the item/key for which you are searching
 -  in_func_count   function to return the number of items in the list
 -  in_func_compare function to compare any two list items
 -  in_func_get     function to return a specific list item by index (0..n)
 
 Returns the index of the matching item, or the index where the item should be inserted
 if it cannot be found (ie. 0 if the list is empty.)
 */
IFSAI_INDEX_TYPE ifsai(void *in_list, void *in_item,
                       IFSAI_INDEX_TYPE (*in_func_count) (void *in_list),
                       int (*in_func_compare) (void *in_list, void *in_left, void *in_right),
                       void* (*in_func_get) (void *in_list, IFSAI_INDEX_TYPE in_index));


#endif
