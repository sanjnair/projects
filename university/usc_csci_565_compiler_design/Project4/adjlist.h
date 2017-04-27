#if !defined __adjlist_H__
#define __adjlist_H__

#include "adjmatrix.h"

//forward declare the class
class AdjList;

typedef std::vector<AdjList> AdjListVector;
typedef std::list<my_simple_reg> MyRegList;

/**
 * Encapsulates the adjlist  of the node
 */
class AdjList  {
public:

  /** 
   * Constructs a AdjList
   */
  AdjList(); 
   
  /**
   * Destroys the adjlist 
   */
  ~AdjList ();

  /**
   * Builds the adjacency list, 
   */
  static void build( const AdjMatrix &adjMatrix,
                     IntMap &regMap,
                     AdjListVector &adjVector ); 

  /**
   * returns true, if the item is in the reglist
   */
  static bool isItemInRegList( const MyRegList &list,
                               const my_simple_reg &reg );

  /**
   * Returns the position of the item in the adjvector
   */
  static int getPos( const AdjListVector &adjVector,
                     const my_simple_reg &reg );

  /**
   * removes the item form the list
   */
  static void removeItemFromList( MyRegList &list,
                                  const my_simple_reg &reg );

  /**
   * returns true, if the regs are equal
   */
  static bool isRegEqual( const my_simple_reg &reg1,
                          const my_simple_reg &reg2 );

  /**
   * prints the lower triangular matrix
   */
  static void print( const AdjListVector &adjVector,
                     FILE *fd );

  //----------------------------------------------------
  my_simple_reg _reg;
  int _nints;
  int _color;
  int _disp;
  float _spcost;
  bool _spill;
  MyRegList _adjnds;
  MyRegList _rmvadj;
};

#endif //__adjlist_H__
