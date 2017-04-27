#if !defined __adjmatrix_H__
#define __adjmatrix_H__

#include "mysimple.h"
#include "prjsupport.h"
#include "bitvector.h"
#include "web.h"
#include "regspec.h"

/**
 * Encapsulates the adjmatrix  of the node
 */
class AdjMatrix  {
public:

  /** 
   * Constructs a AdjMatrix
   */
  AdjMatrix( WebList &webList, 
             BitVectorMap &inMap,
             BitVectorMap &outMap ); 
   
  /**
   * Destroys the adjmatrix 
   */
  ~AdjMatrix ();

  /**
   * builds the adjacency matrix
   */
  void build();

  /**
   * Returns true if two symbolic registers interfere with
   * each other
   */
  bool isSymRegInterfere( int reg1, int reg2 );

  /**
   * returns a new adjmatrix for the coalesced register.
   * It is the caller's reposibility to delete the 
   * newly created object
   */
  AdjMatrix *getCoalescedMatrix( my_simple_reg *coalescedReg,
                                 my_simple_reg *coalescedWithReg );

  /**
   * Sets the row of the adjacency matrix. The size of the
   * vector must be same as the matrix dimension
   */
  bool setRow( int row, BoolVector& vctRow, bool coalesced = false );

  /**
   * returns true/false value for given row and column
   */
  bool getValue( int row, int col ) const;

  /**
   * returns the weblist
   */
  WebList getWebList() const;

  /**
   * Builds the spill interference. This method looks at the
   * weblist and constraints to redo the machine register 
   * interference.
   */
  void buildSpillInterference();

  /**
   * prints the lower triangular matrix
   */
  void print( FILE *fd, bool lower = false ) const;

private:
  /**
   * builds the adjacency matrix
   */
  void build( const ConstraintVector &vct );

  /**
   * Builds the interference for the live variable map
   */
  void buildInterference( BitVectorMap &bitMap );
  
  /**
   * Builds the interference for the bit vector
   */
  void buildInterference( BitVector &bitVector );

  /**
   * Builds the machine register to symbolic register
   * interference
   */
  void buildRegisterInterference( const ConstraintVector &vct );

  /**
   * Makes the constraint for the Machine Register that
   * has no constraints
   */
  void interferenceWithNoConstraints( int machineReg,
                                      const ConstraintVector &vct );

  /**
   * Makes the constraint for the Machine Register that
   * has constraints
   */
  void interferenceWithConstraints( int machineReg,
                                    const ConstraintVector &vct );

  /**
   * returns true if the machine register has constraint
   * in the reg file
   */
  bool hasConstraints(int machineReg, 
                      const ConstraintVector &vct );

  /**
   * Unions the boolean vector
   */
  BoolVector unionVector( BoolVector &b1, 
                          BoolVector &b2,
                          int startFrom = 0 );

  /**
   * Removes an item from the BoolVector identified by
   * the index
   */
  void removeItemFromVector( BoolVector &vct, int pos );

  
  int _dim;
  BoolMatrix _adjMatrix;
  WebList _webList;
  BitVectorMap _inMap;
  BitVectorMap _outMap;
  IntMap _mRegIndexMap; //symbolic register to index map
  IntMap _sRegIndexMap; //symbolic register to index map
};

#endif //__adjmatrix_H__
