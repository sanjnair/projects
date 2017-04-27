#include "digraph.h"
#include "prjutil.h"
#include "prj2.h"
#include "myprint.h"
#include "bitvector.h"

typedef map<int, BitVector> BitVectorMap;

/**
 * print instructions
 */
void printInstructions(FILE *fd, DiGraph &graph, const char* header ) {
  fprintf( fd,"========================================================================\n");
  fprintf( fd, "%s\n\n",header); 
  
  Vertex *vertex_p = 0;
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    my_simple_instr *myInstr = vertex_p->_firstInstr;
    while( myInstr != 0 ) {
      fprint_myInstr( fd, myInstr);
      myInstr = myInstr->next;
    }
  }
  fprintf(fd, "Epilogue\n\n");
}



/**
 * prints the Bit vector expression
 */
void printExpression( FILE *fd, 
                      const char* header,
                      BitVectorMap &map,
                      ExpressionVector *expList ) { 
  
  fprintf( fd,"========================================================================\n");
  fprintf( fd, "%s\n\n",header); 
  fprintf(fd, "Block\t%s\n", header);

  for( int j=0; j < map.size(); j++ ) {
    fprintf(fd, "%d\t", j);
    BitVector bitVector = map[j];
    fprintf( fd, "{ ");
    bool addComma = false;
    for( unsigned int i=0; i< bitVector.size(); i++ ) {
      if( bitVector.item(i) ) {
        if( addComma) {
          fprintf(fd, ", ");
        }
        if( 0 != expList ) {
           fprintf(fd, "(");
           fprint_expression( fd,(*expList)[i]);
           fprintf(fd, ")");
        } else {
           fprintf(fd, "%d", (i+1));
        }
        addComma = true;
      }  
    }
    fprintf( fd, " }\n");
  }
  fprintf( fd, " \n");
}

/**
 * prints expression list
 */
void printExpressionList( FILE *fd, ExpressionVector &list, const char *header ) {
  if(header) {
    fprintf( fd,"========================================================================\n");
    fprintf( fd, "%s\n\n", header);
  }

  if( list.size() == 0 ) {
    fprintf(fd, "None\n");
  } else {
    ExpressionVector::iterator it;
    int count = 0;
    for( it = list.begin(); it != list.end(); it++ ) {
      count++;
      fprintf( fd, "%d: ", count);
      fprint_expression( fd, *it );
      fprintf( fd, "\n");
    }
  }
}

/**
 * Returns the in Bit vector
 */
void getInBitVector( DiGraph &graph,
                     int currentNode,
                     BitVectorMap &outMap,
                     BitVector &bitVector ) {

  IntList predList = graph.getPredecessorList(currentNode);  
  IntList::const_iterator it = 0; 
  int count = 0;

  for( it = predList.begin(); it != predList.end(); it++ ) {
    if( 0 == count ) {
      bitVector = outMap[*it];
    } else {
      bitVector = bitVector * outMap[*it];
    }  
    count++;
  }
}

/**
 * Compute Available Expression
 *
 */
void availableExpression( DiGraph &graph, 
                          BitVectorMap &genMap,
                          BitVectorMap &killMap,
                          BitVectorMap &inMap,
                          BitVectorMap &outMap ) {

  int nodes = graph.size();
  if( nodes == 0 ) {
    return;
  }
  int i=0;
  int vectorSize = genMap[0].size();
  //initialize
  inMap[0] = BitVector(vectorSize); 
  outMap[0] = genMap[0];

  BitVector bitVector(vectorSize);
  BitVector oldOut(vectorSize);

  bitVector.clear(true); 
  for( i=1; i<nodes; i++ ) {
    outMap[i] = bitVector - killMap[i]; 
  } 
  bool change = true;

  while(change) {
    change = false;
    for( i=1; i<nodes;i++) {
      getInBitVector(graph, i, outMap, bitVector);     
      inMap[i] = bitVector;
     
      oldOut = outMap[i];
      outMap[i] = genMap[i] + (inMap[i] - killMap[i]); 

      if( ! (outMap[i] == oldOut) ) {
        change = true;
      }
    } 
  }
}
                      
/**
 * Gets all expressions
 */
void getAllExpressions( DiGraph &graph,
                        ExpressionVector &expList ) {
  Vertex *vertex_p = 0;
 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    vertex_p->getAllExpressions(expList);
  }
}

/**
 * Performs available expression analysis
 *
 */
void expressionAnalysis( DiGraph& graph,
                         FILE *fd, 
                         ExpressionVector &expList,
                         BitVectorMap &inMap,
                         bool print ){
  Vertex *vertex_p = 0;

  if(print) {
    printExpressionList( fd, expList,"Expressions");
  }

  BitVector genVector(expList.size());
  BitVector killVector(expList.size());
  BitVectorMap genMap;
  BitVectorMap killMap;

  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    genVector.clear();
    vertex_p = *it;
    vertex_p->getGenExpressions(expList, genVector);
    vertex_p->_block->num; 
    genMap[vertex_p->_block->num] = genVector;
    
    killVector.clear();
    vertex_p->getKilledExpressions(expList, genVector, killVector);
    killMap[vertex_p->_block->num] = killVector;
  }
  if(print) {
    printExpression( fd, "Generated Expressions", genMap, 0 );
    printExpression( fd, "Killed Expressions", killMap, 0 );
  }

  BitVectorMap outMap;
  availableExpression( graph, genMap, killMap, inMap, outMap );

  if(print) {
    printExpression( fd, "Available Expressions", outMap, &expList );
  }
  //if(! print) {
  //  printExpression( fd, "In Map", inMap, &expList );
  //}
}

/**
 * Perform Expression Analysis
 */
void performExpressionAnalysis( DiGraph &graph, FILE *fd ) {

  ExpressionVector expList;
  getAllExpressions(graph, expList);
  BitVectorMap inMap; 
  expressionAnalysis( graph, fd, expList, inMap, true);  
}

/**
 * Combine the instructions
 */
void combineInstr( DiGraph& graph, FILE* fd ) {

  int instrLine = 0;
  int combinedLine = 0;
  Vertex *vertex_p = 0;

  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    vertex_p->combineInstr( instrLine, combinedLine );
  }
  printInstructions( fd, graph, "After Instruction Combining");
}

/**
 * Converts the graph of blocks to graph of statements
 *
 */
void convertToStatementGraph( DiGraph &graph,
                              DiGraph &instrGraph,
                              FILE *fd ) {

  //Create a new graph and make vertex for each
  //instruction
  Vertex *instrVertex_p = 0;
  Vertex *vertex_p = 0;
  IntMap firstInstrMap; // map of block no. vs first instruction.
  IntMap lastInstrMap;  // map of block no. vs last instruction. 
  int lastBlockNum = -1;
  struct basic_block *bb = 0;
  my_simple_instr *instr = 0;
  my_simple_instr *newInstr = 0;
  
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);

    instr = vertex_p->_firstInstr;
    if( 0 == instr) {
      lastBlockNum++;
      bb = new basic_block;  
      bb->num = lastBlockNum;
      instrGraph.addVertex(bb);
      firstInstrMap[vertex_p->_block->num] = lastBlockNum; 
      lastInstrMap[vertex_p->_block->num] = lastBlockNum; 
    }else {
      int count = 0;
      while( instr != 0 ) {
        lastBlockNum++;
        bb = new basic_block;  
        assert(bb);
        bb->num = lastBlockNum;
        instrGraph.addVertex(bb);
        //TODO, currently ignoring the memory managment
        //BEWARE!!!! IF you delete my_simple_instr struct,
        //THIS WILL BOMB!
        //newInstr = createMySimpleInstr();
        newInstr = copyMySimpleInstr(instr);
        assert(newInstr);
        //*newInstr = *instr;
        //newInstr->next = 0; 
        //newInstr->prev = 0;
        instrVertex_p = instrGraph.getVertex(lastBlockNum);
        assert(instrVertex_p);
        instrVertex_p->_firstInstr = newInstr; 

        if( count == 0 ) {
          firstInstrMap[vertex_p->_block->num] = lastBlockNum; 
        }else {
          //add the edge within the block, except start and end node
          instrGraph.addEdge((lastBlockNum -1), lastBlockNum);
        }
        count++; 
        instr = instr->next;
      }
      lastInstrMap[vertex_p->_block->num] = lastBlockNum; 
    }   
  }

  //now connect the edges for block to block 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
    int blockNum = vertex_p->_block->num;
    int fromNode = lastInstrMap[blockNum];

    //now get successor for each block, and add edges between the new nodes
    IntList succList = graph.getSuccessorList(blockNum);

    for( IntList::const_iterator listIt = succList.begin();
         listIt != succList.end();
         listIt++ ) {
      int toNode = firstInstrMap[*listIt];

      instrGraph.addEdge(fromNode, toNode);
    }
  }
}

/**
 * Gets the expression vertex for the given node
 */
Vertex* getExpressionVertex( DiGraph &graph, 
                             int nodenum,
                             Expression& expr ) {
  Vertex *vertex_p = graph.getVertex(nodenum); 
  assert( vertex_p);

  my_simple_instr *instr = vertex_p->_firstInstr;
  if( 0 != instr ) {
    if(vertex_p->isExpression(instr)){
       Expression tempExpr(instr->oper, instr->src1, instr->src2);
       if( expr == tempExpr ) {
         return vertex_p;
       }
    } 
  } 
  //look in predecessors, except itself
  IntList list = graph.getPredecessorList(nodenum);

  for(IntList::const_iterator it=list.begin();
      it != list.end(); 
      it++ ) {
    if( *it != nodenum ) {
      Vertex *v = 0;
      v = getExpressionVertex( graph,
                               *it,
                               expr ); 
      if( 0 != v ) {
        return v;
      }
    }
  }
  return 0;
}

/**
 * Performs the CSE elimination algorithm
 */
performCSE( DiGraph &graph, FILE *fd) {

  DiGraph instrGraph;
  Vertex *vertex_p = 0;
  convertToStatementGraph( graph, instrGraph, fd );
  //instrGraph.printGraph(fd);

  ExpressionVector subExpressionList;
  BitVectorMap inMap; 
  ExpressionVector expList;
  getAllExpressions(instrGraph, expList);
  expressionAnalysis( instrGraph, fd, expList, inMap , false);  
  IntSet predSet;
  bool cseDone = false;

  for( VertexVector::const_iterator it = instrGraph.begin();
       it != instrGraph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);

    predSet.clear();
    my_simple_instr *instr = vertex_p->_firstInstr;
    if( instr != 0 ) {
      if( vertex_p->isExpression(instr)) {
        Expression expr( instr->oper, instr->src1, instr->src2);
        int pos = vertex_p->getPos( expList, expr );
        assert(pos >= 0);
        BitVector bitVector = inMap[vertex_p->_block->num]; 
        if( bitVector.item(pos) ) {
          //search this expression in the predecessors
          IntList list = instrGraph.getPredecessorList(vertex_p->_block->num);
          simple_reg *newReg = 0; 
          my_simple_reg *newMyReg = 0;
          Vertex *exprVertex = 0;

          for(IntList::const_iterator listit=list.begin(); 
              listit != list.end();  
              listit++ ) {
            if( *listit != vertex_p->_block->num ) { 
              exprVertex = getExpressionVertex( instrGraph, *listit, expr );
              assert(exprVertex);
              //ignore if any block comes back to itself
              if( exprVertex->_block->num != vertex_p->_block->num ) {
                if(! Vertex::isItemInList( subExpressionList, expr )) {
                  subExpressionList.push_back(expr);
                }
                //put it in the predSet so that we get unique predecessors.
                predSet.insert( exprVertex->_block->num );
                cseDone = true;
              }
            }
          } 
          //now traverse the set and add a new statement u
          if( predSet.size() > 0 ) {
            newReg = new_register( simple_type_void, PSEUDO_REG );
            newMyReg = 0;
            for( IntSet::const_iterator predIt = predSet.begin();
                 predIt != predSet.end(); predIt++ ) {
              exprVertex = instrGraph.getVertex( *predIt);
              assert(exprVertex);
              newMyReg = createMySimpleReg();
              newMyReg->reg = newReg; 
              exprVertex->addCSEInstr(newMyReg);
            }
            newMyReg = createMySimpleReg();
            newMyReg->reg = newReg; 
            vertex_p->modifyCSEInstr( newMyReg);
          }
        }
      }
    }
  }
  if( cseDone) {
    int instrLine = 0;
    for( VertexVector::const_iterator it = instrGraph.begin();
         it != instrGraph.end(); it++ ) {
      vertex_p = *it;
      vertex_p->numberInstructions(instrLine);
    }
  }
  printExpressionList( fd, subExpressionList, "Common Sub-Expressions");
  printInstructions( fd, instrGraph, "After CSE Elimination");
}

/**
 * exectues the project 2 requirements.
 */
void execute( struct basic_block *block, FILE* fd ) {

  struct basic_block* p = 0;
  struct basic_block* r = 0;
  DiGraph graph; 

  //add the first node
  graph.addVertex(block);
  r = block->next;

  for( p=r; p; p=p->next ) { 
    graph.addVertex(p); 
  }
  for( p=block; p; p=p->next ) {
    for(int i=0; i<p->link_num; i++ ) { 
      graph.addEdge( p->num, p->links[i]->num ); 
    } 
  }
  
  //combine instructions 
  combineInstr( graph, fd );
  //Compute Expressions
  performExpressionAnalysis( graph, fd);
  
  //perform common subexpression elimination
  performCSE( graph, fd);
}

