#include <iostream>
#include "bitvector.h"

/**
 * Constructs an empty bit vector
 */
BitVector::BitVector() {
  _size = 0;
  _boolArray = new bool[0];
}

/**
 * Constructs a BitVector
 */
BitVector::BitVector(unsigned int size) {
   _size = size;
  _boolArray = new bool[size];
  clear();
} 
  
/**
 * Destroys the expression
 */
BitVector::~BitVector() {

  if( 0 != _boolArray) {
    delete [] _boolArray;
    _boolArray = 0;
    _size = 0;
  }
} 
 
/**
 * Copy C'tor
 */ 
BitVector::BitVector(const BitVector& rhs) {

  _size = rhs._size;
  _boolArray = new bool[_size];     
  for(int i=0; i<_size;i++){
    _boolArray[i] = rhs._boolArray[i];
  }
}
 
/**
 * Assignment operator
 */
BitVector BitVector::operator= (const BitVector &rhs) {

  if( this == &rhs ) {
    return *this;
  }

  _size = rhs._size;
  delete [] _boolArray;

  _boolArray = new bool[_size];
  for(int i=0; i<_size;i++){
    _boolArray[i] = rhs._boolArray[i];
  }
  return *this;
}

/**
 * Clear the bit vector
 */
void BitVector::clear( bool bit) {
  for(int i=0; i<_size;i++) {
    _boolArray[i] = bit;
  }
}
 
/**
 * sets the item in the Bit Vector 
 */
void BitVector::setItem( unsigned int pos, bool item) {
  if( pos < _size ) {
    _boolArray[pos] = item;
  }
}
 
/**
 * Returns the item from the Bit Vector
 */
bool BitVector::item( unsigned int pos ) const{
  if( pos < _size ) {
    return _boolArray[pos];
  }
  return false;
}

/**
 * Overload the == operator
 */
bool BitVector::operator== (const BitVector &rhs) const {
  bool status = true;

  if( _size != rhs.size() ) {
    status = false;
  } else {
    for( int i=0; i<_size; i++ ) {
      if(item(i) != rhs.item(i) ) {
        status = false;
        break;
      } 
    }
  }

  return status;
} 
 
/**
 * Union operator
 */
BitVector BitVector::operator+ (const BitVector &rhs) {
  BitVector vct(_size);
  if( _size == rhs.size() ) { 
    for( int i=0; i<_size; i++ ) {
      vct.setItem(i, item(i) || rhs.item(i));
    }
  }
  return vct;
}
 
/**
 * Intersection operator
 */ 
BitVector BitVector::operator* (const BitVector &rhs) {
  BitVector vct(_size);
  if( _size == rhs.size() ) { 
    for( int i=0; i<_size; i++ ) {
      vct.setItem( i, item(i) && rhs.item(i));
    }
  }
  return vct;
}

/**
 * Minus operator
 */ 
BitVector BitVector::operator- (const BitVector &rhs) {
  BitVector vct(_size);
  if( _size == rhs.size() ) { 
    for( int i=0; i<_size; i++ ) {
      vct.setItem( i, item(i) && (! rhs.item(i)));
    }
  }
  return vct;
}

/**
 * prints the BitVector
 */
void BitVector::print( FILE *fd, unsigned int sep ) {
  for( int i=0; i<_size; i++ ) {
    fprintf(fd, "%d", _boolArray[i] ); 
    if( (-1 != sep) && 
        (((i+1) % sep) == 0) ) {
      fprintf(fd, " " ); 
    }
  }
}
