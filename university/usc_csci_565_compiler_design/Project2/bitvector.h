#if !defined __bitvector_H__
#define __bitvector_H__

#include <stdio.h>
/**
 * Encapsulates the bitvectorof the node
 */
class BitVector {
public:

  /** 
   * Constructs an empty BitVector
   */
  explicit BitVector();

  /** 
   * Constructs a BitVector
   */
  explicit BitVector(unsigned int size);
   
  /**
   * Destroys the expression
   */
  ~BitVector();

  /**
   * Copy C'tor
   */ 
  BitVector(const BitVector& rhs);

  /**
   * Assignment operator
   */
  BitVector operator= (const BitVector &rhs);

  /**
   * Clear the bit vector
   */
  void clear(bool bit = false);  
 
  /**
   * sets the item in the Bit Vector
   */
  void setItem( unsigned int pos, bool item);

  /**
   * Returns the item from the Bit Vector
   */
  bool item( unsigned int pos ) const;

  /**
   * Overload the == operator
   */
  bool operator== (const BitVector &rhs) const;

  /**
   * Union operator
  */
  BitVector operator+ (const BitVector &rhs);

  /**
   * Intersection operator
  */
  BitVector operator* (const BitVector &rhs);

  /**
   * Minus operator
  */
  BitVector operator- (const BitVector &rhs);

  /**
   * Returns the size of the BitVector
   */
  inline unsigned int size() const{
    return _size;
  }

  /**
   * prints the BitVector
   */
  void print( FILE *fd );

private:
  bool *_boolArray;
  unsigned int _size;

};

#endif //__bitvector_H__
