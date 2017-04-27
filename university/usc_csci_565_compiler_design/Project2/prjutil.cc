#include "prjutil.h"

/**
 * returns true, if item exist in the vector
 *
 */
bool isItemInList( const IntList& list, int item ) {
	bool status = false;

	for( IntList::const_iterator it = list.begin();
	     it != list.end();
		 it++ ) { 
		
		if( *it == item ) {
			status = true;
			break;
		}
	}
	return status;
}


/**
 * Performs a set union
 */
IntList setUnion( const IntList& s1, const IntList& s2 ) {
	IntList retList;
	IntList::const_iterator it;
	
	for( it=s1.begin(); it != s1.end(); it++ ) {
		if(! isItemInList(retList, *it )) {
			retList.push_back( *it );
		}
	} 
	for( it=s2.begin(); it != s2.end(); it++ ) {
	   if(! isItemInList(retList, *it )) {
			retList.push_back( *it );
	   }
	} 
   return retList;
}

/**
 * Performs set intersection
 */
IntList setIntersection( const IntList& s1, const IntList& s2 ){
   IntList retList;

   IntList::const_iterator it;
   for( it=s1.begin(); it != s1.end(); it++ ) {
       if(isItemInList(s2, *it)) { 
           retList.push_back( *it );
       }
   }
   return retList;
}

/**
 * Performs set minus
 */
IntList setMinus( const IntList& s1, const IntList& s2 ){
   IntList retList;
 
   IntList::const_iterator it;
   for( it=s1.begin(); it != s1.end(); it++ ) {
	   if( ! isItemInList( s2, *it) ) {
           retList.push_back( *it );
       }
   }
   return retList; 
}

/**
 * returns true if two sets are equal
 */
bool setEqual( const IntList& s1, const IntList& s2 ) {
   bool status = true;

   if( s1.size() == s2.size() ) {
       IntList::const_iterator it;
       for( it=s1.begin(); it != s1.end(); it++ ) {
           if( ! isItemInList( s2, *it) ) {
               status = false;
               break;
           }
       }
   } else {
       status = false;
   }
   return status;
}

/**
 * returns true if first set is in the second set
 */
bool setIn( const IntList& s1, const IntList& s2 ) {
	bool status = true;

	IntList::const_iterator it;
	for( it=s1.begin(); it != s1.end(); it++ ) {
	   if( ! isItemInList(s2,  *it)  ) {
		   status = false;
		   break;
	   }
	}

	return status;
}

/**
 * Converts List to set
 *
 */
void convertListToSet(
				const IntList& list,
				IntSet& set) {

	for( IntList::const_iterator it = list.begin();
		 it != list.end();
		 it++ ) {
		
		set.insert(*it);
	}
}


/**
 * Prints the header
 *
 */
void printHeader( const char* header1, const char* header2, FILE* fd ) {
	fprintf( fd, "\n========================================================================\n");
	if( header1 ) {
		fprintf( fd, "%s\n\n", header1);
	}
	if( header2 ) {
		fprintf( fd, "%s\n", header2);
	}
}

/**
 * prints the set
 */
void printSet( const IntSet& set, FILE* fd, const char* sep, bool asc ) {

	IntStack s;

	if( set.size() == 0 ) {
		fprintf( fd, "None");
		return;
	}
	int count = 0;
	int size = set.size();

	for( IntSet::const_iterator it = set.begin();
		 it != set.end();
		it++ ) {

		if( !asc ) {
			s.push(*it);
		} else {
			fprintf( fd, "%d", *it );
			if( sep ) {
				if( count < (size-1) ) {
					fprintf( fd, "%s", sep );
				}
			}
		}
		count++;
	}
	
	if( !asc ) {
		count = 0;
		size = s.size();
		while( !s.empty() ) {
			fprintf( fd, "%d", s.top() );
			if( sep ) {
				if( count < (size-1) ) {
					fprintf( fd, "%s", sep );
				}
			}
			s.pop();
			count++;
		}
	}
}

/**
 * prints the list
 */
void printList( const IntList& list, FILE* fd ) {

    for( IntList::const_iterator it = list.begin();
         it != list.end();
         it++ ) {
		fprintf( fd, "%d  ", *it );
    }
}