#include <stack>
#include <stdio.h>
#include "digraph.h"


#if !defined __prjutil_H__
#define __prjutil_H__

#include "digraph.h"

/**
 * Performs a set union
 */
IntList setUnion( const IntList& s1, const IntList& s2 );

/**
 * Performs set intersection
 */
IntList setIntersection( const IntList& s1, const IntList& s2 );

/**
 * Performs set minus
 */
IntList setMinus( const IntList& s1, const IntList& s2 );

/**
 * returns true if two sets are equal
 */
bool setEqual( const IntList& s1, const IntList& s2 );

/**
 * returns true if first set is in the second set
 */
bool setIn( const IntList& s1, const IntList& s2 );

/**
 * returns true, if item exist in the vector
 *
 */
bool isItemInList( const IntList& vct, int item );

/**
 * Converts List to set
 *
 */
void convertListToSet(const IntList& list,IntSet& set);

/**
 * Prints the header
 *
 */
void printHeader( const char* header1, const char* header2, FILE* fd );

/**
 * prints the set
 */
void printSet( const IntSet& set, FILE* fd, const char* sep = "  ", bool asc = false );

/**
 * prints the list
 */
void printList( const IntList& list, FILE* fd );

#endif __prjutil_H__
