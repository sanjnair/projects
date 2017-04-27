#include "digraph.h"
#include "prjutil.h"
#include "prj4.h"
#include "myprint.h"
#include "bitvector.h"
#include "regspec.h"
#include "adjmatrix.h"
#include "adjlist.h"

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
performCSE( DiGraph &graph, 
            DiGraph &instrGraph,
            FILE *fd ) {

  Vertex *vertex_p = 0;
  convertToStatementGraph( graph, instrGraph );
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
              newMyReg->num =newReg->num ; 
              newMyReg->kind =newReg->kind ; 
              exprVertex->addCSEInstr(newMyReg);
            }
            newMyReg = createMySimpleReg();
            newMyReg->reg = newReg; 
            newMyReg->num =newReg->num ; 
            newMyReg->kind =newReg->kind ; 
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
  //printExpressionList( fd, subExpressionList, "Common Sub-Expressions");
  printInstructions( fd, instrGraph, "After CSE Elimination");
}

/**
 * exectues the project 2 requirements.
 */
void execute( struct basic_block *block, FILE* fd ) {

  struct basic_block* p = 0;
  struct basic_block* r = 0;
  DiGraph *graph = new DiGraph(); 

  //add the first node
  graph->addVertex(block);
  r = block->next;

  for( p=r; p; p=p->next ) { 
    graph->addVertex(p); 
  }
  for( p=block; p; p=p->next ) {
    for(int i=0; i<p->link_num; i++ ) { 
      graph->addEdge( p->num, p->links[i]->num ); 
    } 
  }
  
  //evaluate the register file
  if( ! RegSpec::getInstance()->evalParam(graph->size()) ) {
    exit (-1);
  }

  //combine instructions 
  combineInstr( *graph, fd );
  //Compute Expressions
  //performExpressionAnalysis( graph, fd);
  
  //perform common subexpression elimination
  DiGraph *instrGraph = new DiGraph();
  performCSE( *graph,*instrGraph, fd);

  convertToBlockGraph( *instrGraph, *graph );

  DiGraph *ssaGraph = new DiGraph();
  *ssaGraph = *graph;
  //convert the statement graph to block graph
  convertToBlockGraph( *instrGraph, *ssaGraph );

  //compute Dominance Frontier
  Vertex *vertex_p = graph->getVertex( 0 );
  assert(vertex_p);
  int dominStartNode = vertex_p->_block->num;

  IntList emptyList;
  VertexListVector domin( ssaGraph->size(), emptyList);
  dom_comp(*ssaGraph,dominStartNode,domin );
  VertexListVector idomin( ssaGraph->size(), emptyList);
  idom_comp( *ssaGraph, domin, dominStartNode, idomin );
 
  VertexListVector dfList( ssaGraph->size(), emptyList);
  calculate_dominance_frontier( *ssaGraph,
                                dominStartNode,
                                idomin,
                                dfList );
  printVertexList( *graph, 
                   dfList, 
                   "Dominance Frontier",
                   "Block\tDF",
                   fd );

  getMinimalSSA( *ssaGraph, dfList, idomin );
  printHeader( "Minimal SSA form", 0, fd );
  printInstructions( fd, *ssaGraph);

  //clear the ssa graph
  delete ssaGraph;

  ssaGraph = new DiGraph();

  //copy the CSE graph and rename the old instr number as new number
  *ssaGraph = *graph;
  convertToBlockGraph( *instrGraph, *ssaGraph );

  //delete the instr graph
  delete instrGraph;


  numberOldInstructions( *ssaGraph );
  getMinimalSSA( *ssaGraph, dfList, idomin );
  //printHeader( "1 Minimal SSA", 0, fd );
  //printInstructions( fd, *ssaGraph);

  DuChainVector duChainList;
  getDuChain(*ssaGraph, duChainList );
  printDuChain( duChainList, fd );

  delete ssaGraph;
  ssaGraph = 0;

  allocateRegisters( *graph, duChainList, fd );

  /*
  WebList webList;
  makeWebs(duChainList,webList);
  printWebs( webList, fd );

  replaceWithWebs( *graph, webList );
  printHeader( "After replacing by webs", 0, fd );
  printInstructions( fd, *graph);

  BitVectorMap inMap;
  BitVectorMap outMap;
  
  performLVA(*graph, webList, inMap, outMap, fd );

  AdjMatrix *matrix = new AdjMatrix( webList, inMap, outMap );
  matrix->build();
  printAdjMatrix(*matrix, fd );
  //matrix->print(fd);

  AdjMatrix *newMatrix = 0;
  IntPairVector* regPairVector = new IntPairVector();
  newMatrix = coalesceRegister( *graph, *matrix, *regPairVector );

  printCoalescing( *regPairVector, fd );
  delete regPairVector;
  fprintf(fd, "After register coalescing\n");
  numberInstructions(*graph);
  printInstructions( fd, *graph);

  if( 0 != newMatrix ) {
    printAdjMatrix(*newMatrix, fd );
    delete matrix; 
    matrix = newMatrix;
  }else {
    printAdjMatrix(*matrix, fd );
  }
  //matrix->print(fd);

  IntMap *regMap = new IntMap();
  calculateSpillCost( *graph, *regMap );
  printSpillCost(*regMap, fd );

  colorGraph( *graph, *matrix, *regMap, fd );

  delete matrix;
  delete regMap;
  */



  delete graph;
}

