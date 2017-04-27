#if !defined __vertex_H__
#define __vertex _H__

#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <list>
#include <string>

extern "C"
{
#include "bb1.h"
}

#include "mysimple.h"
#include "expression.h"
#include "bitvector.h"
//----------------------------------------------
typedef std::list<int> IntList;
typedef std::map<int,int> IntMap;
typedef std::vector<int> IntVector;
typedef std::vector<bool> BoolVector;
typedef std::set<int> IntSet;
typedef std::stack<int> IntStack;
typedef std::queue<int> IntQueue;
typedef std::vector<Expression> ExpressionVector;

typedef enum {
  TREE, 
  FORWARD, 
  BACK, 
  CROSS, 
  UNKNOWN
} EdgeType; 

struct Edge {
  int startNode;
  int endNode;
  EdgeType type;
  bool operator==(const Edge& y) {return (this->startNode == y.startNode);}
  bool operator<(const Edge& y) {return this->startNode < y.startNode;}
};

typedef std::vector<Edge> EdgeVector;
typedef std::list<Edge> EdgeList;
typedef std::vector<my_simple_instr *> MyInstrList;

/**
 * Encapsulates the vertex of the node
 */
class Vertex {
public:

  /** 
   * Constructs a Vertex with id and block
   */
  Vertex( struct basic_block *block );
   
   /**
    * Destroys the vertex
    */
  ~Vertex();

  /**
   * Combines the instructions
   * @param    Last instruction line number
   * @param    Last combined line number
   */
  void combineInstr(int &lastInstrLineNo,
                    int &lastCombinedLineNo );

  /**
   * number the instructions
   */
  void numberInstructions( int &lastInstrNumber );

  /**
   * Adds the CSE Instruction to the register
   */
  void addCSEInstr( my_simple_reg *reg );

  /**
   * Modifieds the CSE Instruction to the register
   */
  void modifyCSEInstr( my_simple_reg *reg );

  /**
   * Returns all the expression in a block
   */
  void getAllExpressions( ExpressionVector& list );

  /**
   * Returns generated expressions by the block
   */
  void getGenExpressions( const ExpressionVector &list,
                          BitVector &genVector );

  /**
   * Returns the killed expression by the block
   */
  void getKilledExpressions( const ExpressionVector &list,
                             const BitVector &genVector,
                             BitVector &killVector );

  /**
   * returns true, if the given insruction is an expression
   */
  bool isExpression(const my_simple_instr *instr );

  /** 
   * returns true if the item is in the list
   */
  static bool isItemInList( 
                const ExpressionVector &list,
                const Expression &exp );

  /**
   * Returns the position of the expression in the list
   */
  static int getPos( const ExpressionVector &list,
                     const Expression &exp );


  IntList _adjList; // adjacency list
  IntList _prevList; // previous node list
  struct basic_block *_block;
  bool visited;
  my_simple_instr *_firstInstr;
  my_simple_instr *_lastInstr;

private:
  /**
   * Initializes the instruction to my_simple_instr format
   */
  void initInstr( int &lastInstrLineNo ); 

  /**
   * Combines instructions from and to and the result is stored in to
   * returns true, if the instruction is combined.
   */
  bool combineInstrInternal( my_simple_instr *from, my_simple_instr *to );

  /**
   * Returns the instruction where this temporary register is used
   */
  my_simple_instr *findInstr( my_simple_instr *instr,
                              my_simple_reg *dst );


  /**
   * returns true, if the regiter is used as an assignment
   * from the given node.
   */
  bool isGenerated( my_simple_instr *from, my_simple_reg *src );

 
};

#endif //__vertex _H__
