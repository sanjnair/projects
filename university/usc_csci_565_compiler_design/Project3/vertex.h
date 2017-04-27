#if !defined __vertex_H__
#define __vertex _H__

#include "prjsupport.h"
extern "C"
{
#include "bb1.h"
}

#include "mysimple.h"
#include "expression.h"
#include "web.h"
#include "bitvector.h"
//----------------------------------------------

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
  Vertex( struct basic_block *block,
          bool deleteBlock = false );
   
   /**
    * Destroys the vertex
    */
  ~Vertex();

  /**
   * Copy C'tor
   */
  Vertex(const Vertex& rhs);
 
  /**
   * Assignment operator
   */
  Vertex operator= (const Vertex &rhs);

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
   * number the old instruction as the number of new instructions
   */
  void numberOldInstructions();

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
   * Gets the definitions for this block
   */ 
  void Vertex::getDefinitions( IntList &list );

  /**
   * Returns all variables and its kind in the block
   */
  void getAllVariables( IntMap &allVars );

  /**
   * Adds the PHI instruction at the begining of the block
   */
  void addPhiInstr( int regNum, int regKind );

  /**
   * Replace the registers with webs
   */
  void replaceWithWebs( const WebList &webs ); 

  /**
   * Returns the def and use of the vertex. The graph should be
   * a statement graph to get the def and use
   */
  void getDefAndUse( BitVector &def,
                     BitVector &use );

  /**
   * deletes the instruction and re-arranges the link list
   */
  void deleteInstr( my_simple_instr *myInstr );

  /**
   * returns true, if item exist in the vector
   *
   */
  static bool isItemInList( const IntList& list, int item );

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
  bool _deleteBlock;

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

  /**
   * Adds a variable to map. This is a helper method
   */
  void addVariableToMap( 
               IntMap &allVars,
               my_simple_reg *reg );

  /**
   * replaces the current register with the web register
   */
  void replaceWithWebRegister( my_simple_instr *myInstr,
                               my_simple_reg *myReg,
                               const WebList &webs,
                               bool def );

};

#endif //__vertex _H__
