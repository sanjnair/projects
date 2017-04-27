#if !defined __prjutil_H__
#define __prjutil_H__

#include "digraph.h"
#include "duchain.h"
#include "web.h"
#include "adjmatrix.h"
#include "adjlist.h"

typedef std::stack<my_simple_reg> MyRegStack;

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
void printSet( const IntSet& set, FILE* fd, 
               const char* sep = "  ", 
               bool asc = false );

/**
 * prints the list
 */
void printList( const IntList& list, FILE* fd );

/**
 * Print the VertexList
 */
void printVertexList(  
                DiGraph& graph,
                const VertexListVector& list,   
                const char* header1,
                const char* header2,
                FILE* fd,
                bool asc = true );

/**
 * Computes the dominators.
 */
void dom_comp( DiGraph& graph, 
               int r, 
               VertexListVector& domin );

/**
 * Computes immediate dominator
 *
 */
void idom_comp( DiGraph& graph,
                VertexListVector& domin,
                int r,
                VertexListVector& idomin );

/**
 * Computes the post dominators.
 */
void pdom_comp( DiGraph& graph, 
                int r, 
                VertexListVector& domin );

/**
 * returns the set of items the node dominates
 */
void getDominatorSet( VertexListVector& domin,
                      int vertexId,
                      IntSet& set ); 

/**
 * Print the dominator list
 */
void printDom( DiGraph& graph, const VertexListVector& domin, FILE* fd ); 

/**
 * Prints immediate dominators for each block
 *
 */
void printiDom( DiGraph& graph, const VertexListVector& idomin, FILE* fd ); 

/**
 * print instructions
 */
void printInstructions(FILE *fd, DiGraph &graph );

/**
 * Clear the graph instructions. This method will 
 * clear instructions in every node of the graph
 */
void clearGraphInstructions( DiGraph &graph);

/**
 * Number the instructions
 */
void numberInstructions( DiGraph &graph );

/**
 * Number the old instructions
 */
void numberOldInstructions( DiGraph &graph );

/**
 * Converts the Statement graph to original graph.
 * This method only copies the instructions from the
 * statement graph to the original graph. This method 
 * assumes that the original graph passed has all the 
 * node and edge information.
 */
void convertToBlockGraph( DiGraph &statementGraph,
                          DiGraph &blockGraph );


/**
 * Converts the graph of blocks to graph of statements
 */
void convertToStatementGraph( DiGraph &graph,
                              DiGraph &instrGraph );

/**
 * Computes the dominance frontier of each block
 */
void calculate_dominance_frontier( DiGraph &graph,
                                   int block,
                                   const VertexListVector &idomList,
                                   VertexListVector &dfList );

/**
 * Returns the list of nodes that are children of the given node.
 * Children are the nodes that are successor of the given node
 * and the dominator of the successor is the given node.
 */
void getChildren( DiGraph &graph,
                  int block,
                  const VertexListVector &domList,
                  IntList &children );

/**
 * This method returns the immediate dominator 
 * from the given list. If the list is empty,
 * it will return -1.
 */
int getImmediateDom( const IntList &idomList );

/**
 * Returns all variables in the graph
 */
void getAllVariables( DiGraph &graph,
                      IntMap &allVars );


/**
 * Inserts Phi functions
 */
void insertPhiFunctions( 
            DiGraph &graph,
            const VertexListVector &dfList,
            IntList &allDefs,
            IntMap &allVarsMap );

/**
 * Returns the index of the given block in 
 * the list
 */
int getListIndex( const IntList &list,
                  int blockNum ); 

/**
 * Renames  the register with the new name
 */
void renameReg( my_simple_reg *myReg,
                IntStackMap &stackMap );

/**
 * Helper function for renaming variables
 */
void rename( DiGraph &graph,
             int blockNum,
             IntMap &Count,
             IntStackMap &StackMap,
             const VertexListVector &idomList );
             

/** 
 * Rename the variables
 */
void renameVariables( DiGraph &graph,
                      const IntMap &allVarsMap,
                      const VertexListVector &idomList );

/**
 * Returns true if the variable is used
 */
bool isPhiVariableUsed( my_simple_reg *reg1,
                        my_simple_reg *reg2);

/**
 * Returns true, if the definition of phi function
 * is used in the graph
 */
bool isPhiDefinitionUsed( DiGraph & graph,
                          my_simple_reg *myReg );

/**
 * Minimizes the SSA
 */
void minimizeSSA( DiGraph &graph );

/**
 * Gets the minimal SSA
 */
void getMinimalSSA( DiGraph &graph,
                    const VertexListVector &dfList,
                    const VertexListVector &idomList );
                      
/**
 * Method returns if the item is in DuChainList
 */
bool isItemInDuChainList( const DuChainVector &duChainList,
                          my_simple_reg *myReg );


/**
 * returns the DU chain use for the given def
 */
void getDuChainUse( DiGraph &graph,
                    my_simple_reg *defReg, 
                    DuChain &duChain,
                    DuChainVector &visted  );

/**
 * Adds the DuChain
 */
void addDuChain( DiGraph &graph,
                 int def,
                 my_simple_reg *myReg,
                 DuChainVector &duChainList );

/**
 * Get the Vector of DU chains for the given graph
 */
void getDuChain( DiGraph &graph,
                 DuChainVector &duChainList );

/**
 * Prints the reg nos
 */
void printRegNumber( const my_simple_reg *myReg, FILE *fd );

/**
 * prints the DU chain
 */
void printDuChain( const DuChainVector &duChainList, FILE *fd );

/**
 * prints the Webs
 */
void printWebs( const WebList &webList, FILE *fd );

/**
 * Prints the adjacency Matrix
 */
void printAdjMatrix( const AdjMatrix &adjMatrix, FILE *fd );

/**
 * Returns true if the item is in web list
 */
bool isItemInWebList( const WebList &list );

/**
 * Prints the coalescing
 */
void printCoalescing( IntPairVector regPairVector,
                      FILE *fd );

/**
 * Performs set minus on weblists
 */
WebList webSetMinus( const WebList& s1, const WebList& s2 );

/**
 * Make the webs
 */
void makeWebs( const DuChainVector &duChainList,
               WebList &webs );

/**
 * Replaces the registers by webs
 */
void replaceWithWebs( DiGraph &graph,
                    const WebList &webs );

/**
 * Returns the Out Bit vector
 */
void getOutBitVector( DiGraph &graph,
                     int currentNode,
                     BitVectorMap &inMap,
                     BitVector &bitVector ); 
                    
/**
 * Computes the live variable analysis
 */
void liveVariables( DiGraph &graph,
                    BitVectorMap &defMap,
                    BitVectorMap &useMap,
                    BitVectorMap &inMap,
                    BitVectorMap &outMap,
                    unsigned int &iteration );
                     
/**
 * Performs the Live Variable Analysis
 */ 
void performLVA( DiGraph &graph,
                 const WebList &webs,
                 BitVectorMap &inMap,
                 BitVectorMap &outMap,
                 FILE *fd = 0 );

/**
 * returns true if the instruction is the register copy instruction
 */
bool isRegCopyInstr( my_simple_instr* myInstr );

/**
 * Recurssively searches for the definition of the register
 * in all the successors of the current loop. If it can find
 * the definition, it will return true.
 * If search is true, then this block will be 
 * searched for Reg definitions.
 */
bool isRegDefinedInBlock( DiGraph &graph,
                          int blockNum,
                          my_simple_reg *reg1,
                          my_simple_reg *reg2,
                          IntList &visited,
                          bool search );

/**
 * This method starts searching for a definition of reg1/ reg2
 * starting from the instruction pointed by myInstr in the
 * current block. If it finds it, it will return true.
 */
bool isRegDefined( my_simple_instr *myInstr,
                   my_simple_reg *reg1,
                   my_simple_reg *reg2 );



/**
 * Replaces the coalesced register 
 */
void replaceCoalescedReg( my_simple_reg *replaceReg,
                           my_simple_reg *coalescedReg,
                           my_simple_reg *replaceWithReg );

/**
 * Replaces the register with the coalesced register in the graph
 */
void replaceCoalescedRegInGraph( DiGraph &graph, 
                                 my_simple_instr *regCopyInstr );

/**
 * Performs the register coalescing.
 * if the register is coalesced, then
 * this method returns a new instance of
 * the adjacency matrix. Else it will return NULL.
 * The caller MUST de-allocate memory for the returned
 * matrix.
 */
AdjMatrix* coalesceRegister( 
                       DiGraph &graph,
                       AdjMatrix &adjMatrix,
                       IntPairVector &coalescePair );


/**
 * returns true if the instruction is the register copy instruction
 */
bool isRegImmediate( my_simple_instr* myInstr );

/**
 * Calculates the spill cost for the given register
 */
void spillCostReg( my_simple_reg *reg, 
                   int loopCount,
                   int unitCost,
                   IntMap &regMap );

/**
 * returns true if the symReg matches with the regnum
 */
bool isSymRegFound( int regNum, 
                    my_simple_reg *myReg );

/**
 * returns true if the register is used anywhere in the graph
 */
bool isSymRegUsed( int regNum,
                   DiGraph &graph );

/**
 * Calculates the spill cost
 */
void calculateSpillCost( DiGraph& graph,
                         IntMap &regMap );

/**
 * Prints the spill cost
 */
void printSpillCost( IntMap &regMap,
                     FILE *fd );


/**
 * Prints register stack
 */
void printRegStack( const MyRegStack &regStack,
                    FILE *fd );

/**
 * returns true, if the item is in the register stack
 */
bool isItemInRegStack( const MyRegStack &regStack, 
                       const my_simple_reg &reg );

/**
 * move neighbors of i from adjnds to rmvadj and 
 * disconnect node i from its neighbors
 */
void adjustNeighbors( AdjListVector &adjVector,
                      int i );

/**
 * Prunes the interference graph
 */
void pruneGraph( AdjListVector &adjVector,
                 const WebList &webList,
                 MyRegStack &regStack,
                 FILE *fd );

/**
 * returns the min color for the adjList. If the color
 * cannot be assigned, it will return 0
 */
int getMinColor( const AdjListVector &adjVector,
                 my_simple_reg &reg );
                 
/**
 * Assign the registers
 */
bool assignRegs( AdjListVector &adjVector,
                 MyRegStack &regStack,
                 const IntList &alreadySpilledList,
                 IntList &spilledList,
                 FILE *fd );

/**
 * Modifies the code after register coloring
 */
void modifyCode( DiGraph &graph,
                 const AdjListVector &adjVector );

/**
 * Generates the spill code and returns the displacement
 */
int genSpillCode( DiGraph &graph,
                  const AdjListVector &adjVector,
                  int lastdisp,  
                  FILE *fd );

/**
 * Returns new web register list for the old spill list.
 */
IntList getSpilledList( const WebList &webList,
                        const IntList spillList );


/**
 * Allocate registers. This method repeats until, it can 
 * allocate the registers.
 */
void allocateRegisters( DiGraph &graph,
                        DuChainVector &duChainList,
                        FILE *fd );



              
#endif __prjutil_H__
