#include "prjutil.h"
#include "myprint.h"
#include "regspec.h"

/**
 * Performs a set union
 */
IntList setUnion( const IntList& s1, const IntList& s2 ) {
  IntList retList;
  IntList::const_iterator it;

  for( it=s1.begin(); it != s1.end(); it++ ) {
    if(! Vertex::isItemInList(retList, *it )) {
      retList.push_back( *it );
    }
  } 
  for( it=s2.begin(); it != s2.end(); it++ ) {
    if(! Vertex::isItemInList(retList, *it )) {
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
       if( Vertex::isItemInList(s2, *it)) { 
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
	   if( ! Vertex::isItemInList( s2, *it) ) {
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
           if( ! Vertex::isItemInList( s2, *it) ) {
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
	   if( ! Vertex::isItemInList(s2,  *it)  ) {
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
void convertListToSet( const IntList& list,
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

//------------------------------------------------------------------------

/**
 * Computes the dominators.
 */
void dom_comp( DiGraph& graph,  
               int r,  
               VertexListVector& domin ) {
  IntList D;
  IntList T;
  IntList::const_iterator it;
  bool change = true;
  IntList predList;
  IntList setR;
  IntList N;
 
  N = graph.getVertexList(r, true);
 
  setR.push_back(r);
  domin[r]= setR;
 
  IntList N_r = setMinus( N,setR );
 
  for( it=N_r.begin();it !=  N_r.end(); it++ ) {
    domin[*it] = N;
  }
 
  do {
    change = false;
    for( it=N_r.begin();it !=  N_r.end();it++ ) { //for each n, n is *it
      T = N;
      predList = graph.getPredecessorList(*it );
      for( IntList::iterator predIt=predList.begin();  //for each p, p=*predIt
        predIt != predList.end(); predIt++ ) {
        T = setIntersection( T, domin[*predIt]);
      }

      IntList nSet;
      nSet.push_back( *it);
      D = setUnion(nSet,T);

      if( ! setEqual( D, domin[*it]) ) {
        change = true;
        domin[*it] = D;
      }
    }
  }while(change);
} 
 
/**
 * Computes immediate dominator
 *
 */
void idom_comp( DiGraph& graph,
                VertexListVector& domin, 
                int r, 
                VertexListVector& idomin ) {

  IntList::const_iterator it;
  IntList n;
  IntList s;
  IntList t;
  IntList setR;
  IntList N;
  VertexListVector Tmp( graph.size(), n); //initialize to empty list
 
  N = graph.getVertexList(r,true);
 
  //for each n in N do
  //Tmp(n) := Domin(n) - {n}
  for( it=N.begin();it !=  N.end();it++ ) {
    n.push_back(*it);
    Tmp[*it] = setMinus(domin[*it],n);
    n.clear();
  }
 
  setR.push_back(r);
  IntList N_r = setMinus( N,setR );
 
  //for each n in N -{r} do
  for( it=N_r.begin();it !=  N_r.end();it++ ) {
    n.push_back(*it);
    IntList Tmp_set = Tmp[*it]; // Tmp(n)
    for( IntList::const_iterator s_it = Tmp_set.begin();
         s_it != Tmp_set.end(); s_it++ ) {
      s.push_back(*s_it);
      //Tmp(n)-{s}
      //IntSet TmpN_s = setMinus(Tmp[*it],s );
      IntList TmpN_s = setMinus(Tmp_set,s );  // Tmp(n) - {s}
 
      for( IntList::const_iterator t_it = TmpN_s.begin();
           t_it != TmpN_s.end(); t_it++ ) {
           t.push_back( *t_it );
        if( setIn(t, Tmp[*s_it]) ) {
          Tmp[*it] = setMinus( Tmp[*it], t );
        }
        t.clear();
      }
      s.clear();
    }
    n.clear();
  }
  //for each n in N - {r} do
  for( it=N_r.begin();it !=  N_r.end();it++ ) {
       idomin[*it] = (Tmp[*it]);
  }
}
 
/**
 * Computes the post dominators.
 */
void pdom_comp( DiGraph& graph,  
                int r,  
                VertexListVector& domin ) {
  IntList D;
  IntList T;
  IntList::const_iterator it;
  bool change = true;
  IntList predList;
  IntList setR;
  IntList N;
 
  N = graph.getVertexList(r, true);
  setR.push_back(r);
  domin[r]= setR;
 
  IntList N_r = setMinus( N,setR );
  IntList tempList = N_r;
  tempList.sort();
 
  N_r.clear();
  IntList::reverse_iterator revIt;
  for( revIt = tempList.rbegin(); revIt != tempList.rend(); revIt++) {
    N_r.push_back( *revIt );
  }
 
  for( it=N_r.begin();it !=  N_r.end(); it++ ) {
    domin[*it] = N;
  }
 
  do {
    change = false;
    for( it=N_r.begin();it !=  N_r.end();it++ ) { //for each n, n is *it
      T = N;
      predList = graph.getPredecessorList(*it );
      for( IntList::iterator predIt=predList.begin();  //for each p, p=*predIt
           predIt != predList.end(); predIt++ ) {
        //check if it is an empty list, if yes, then it is not in the graph
        if( (domin[*predIt]).size() > 0 ) {
          T = setIntersection( T, domin[*predIt]);
        }
      }
      IntList nSet;
      nSet.push_back( *it);
      D = setUnion(nSet,T);
      if( ! setEqual( D, domin[*it]) ) {
        change = true;
        domin[*it] = D;
        domin[*it] = D;
      }
    }
  }while(change);
}

/**
 * returns the set of items the node dominates
 */
void getDominatorSet( const VertexListVector& domin,
                      int vertexId,
                      IntSet& set ) {
 
  for(int i=0; i< domin.size(); i++ ) {
    if(Vertex::isItemInList(domin[i], vertexId)) {
      set.insert(i);
    }
  }
}

/**
 * Print the dominator list
 */
void printDom( DiGraph& graph, const VertexListVector& domin, FILE* fd ) {
 
  Vertex* vertex_p = 0;
  int vertexId;
  printHeader( "Dominators", "Block\tDominates", fd );
 
  for(int i=0; i< domin.size(); i++ ) {
    vertex_p = graph.getVertex( i);
    vertexId = vertex_p ->_block->num;
 
    fprintf( fd, "%d\t",vertexId );
    IntSet set;
    getDominatorSet(domin,vertexId, set );
    printSet( set, fd );
    printf("\n");
  }
}
 
/**
 * Print the VertexList
 */
void printVertexList( 
                DiGraph& graph, 
                const VertexListVector& list, 
                const char* header1,
                const char* header2,
                FILE* fd,
                bool asc ) {
 
  Vertex* vertex_p = 0;
  int vertexId;
 
  printHeader( header1,header2 ,fd );

  for(int i=0; i< list.size(); i++ ) {
    vertex_p = graph.getVertex( i);
    vertexId = vertex_p ->_block->num;
 
    fprintf( fd, "%d\t",vertexId );
    IntSet set;
    convertListToSet(list[i], set );
    printSet( set, fd, " ",  asc );
    printf("\n");
  }
}

 
/**
 * Prints immediate dominators for each block
 *
 */
void printiDom( DiGraph& graph, const VertexListVector& idomin, FILE* fd ) {
 
  Vertex* vertex_p = 0;
  int vertexId;
  printHeader( "Immediate Postdominator Set", "Block\tImmediate Postdominators", fd );
 
  for(int i=0; i< idomin.size(); i++ ) {
    vertex_p = graph.getVertex( i);
    vertexId = vertex_p ->_block->num;
 
    fprintf( fd, "%d\t",vertexId );
    IntSet set;
    convertListToSet(idomin[i], set);
    printSet( set, fd );
    printf("\n");
  }
}

/**
 * print instructions
 */
void printInstructions(FILE *fd, DiGraph &graph ) {
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
 * Prints the adjacency Matrix
 */
void printAdjMatrix( const AdjMatrix &adjMatrix, FILE *fd ) {
  printHeader( "Adjacency Matrix",0 , fd );
  adjMatrix.print(fd, true );
}

/**
 * Number the instructions
 */
void numberInstructions( DiGraph &graph ) {
  int instrLine = 0;
  Vertex *vertex_p = 0;
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    vertex_p->numberInstructions(instrLine);
  }
}

/**
 * Number the old instructions
 */
void numberOldInstructions( DiGraph &graph ) {
  Vertex *vertex_p = 0;
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    vertex_p->numberOldInstructions();
  }
}

/**
 * Clear the graph instructions. This method will 
 * clear instructions in every node of the graph
 */
void clearGraphInstructions( DiGraph &graph) {
  Vertex *vertex_p = 0;
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    my_simple_instr *myInstr = vertex_p->_firstInstr;
    my_simple_instr *tempInstr = vertex_p->_firstInstr;
    while( myInstr != 0 ) {
      tempInstr = myInstr;
      myInstr = myInstr->next;
      deleteMySimpleInstr(tempInstr);
    }
    vertex_p->_firstInstr = 0;
    vertex_p->_lastInstr = 0;
  }
}

/**
 * Converts the Statement graph to original graph.
 * This method only copies the instructions from the
 * statement graph to the original graph. This method 
 * assumes that the original graph passed has all the 
 * node and edge information.
 */
void convertToBlockGraph( DiGraph &statementGraph,
                          DiGraph &blockGraph ) {

  clearGraphInstructions( blockGraph );  

  Vertex *statementVertex_p = 0;
  Vertex *graphVertex_p = 0;
  int blockNum = 0;

  for( VertexVector::const_iterator it = statementGraph.begin();
       it != statementGraph.end(); it++ ) {
    statementVertex_p = *it;

    my_simple_instr *myInstr = statementVertex_p->_firstInstr;
    my_simple_instr *copyInstr = 0;

    if( 0 != myInstr ) {
      blockNum = myInstr->block_no;
      graphVertex_p = blockGraph.getVertex(blockNum);
      assert(graphVertex_p);

      while( myInstr != 0 ) {
        copyInstr = copyMySimpleInstr( myInstr );
        if( 0 == graphVertex_p->_firstInstr ) {
          graphVertex_p->_firstInstr = copyInstr;
        }else if( 0 == graphVertex_p->_lastInstr ) {
          graphVertex_p->_firstInstr->next = copyInstr;
          copyInstr->prev = graphVertex_p->_firstInstr;
          graphVertex_p->_lastInstr = copyInstr;
        } else {
          graphVertex_p->_lastInstr->next = copyInstr;
          copyInstr->prev = graphVertex_p->_lastInstr;
          graphVertex_p->_lastInstr = copyInstr;
        } 
        myInstr = myInstr->next;
      }
    }
  }
}

/**
 * Converts the graph of blocks to graph of statements
 *
 */
void convertToStatementGraph( DiGraph &graph,
                              DiGraph &instrGraph ) {
 
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
        instrGraph.addVertex(bb, true);
        newInstr = copyMySimpleInstr(instr);
        assert(newInstr);
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
 * This method returns the immediate dominator  
 * from the given list. If the list is empty,
 * it will return -1.
 */
int getImmediateDom( const IntList &idomList ) {
  int retVal = -1;

  assert( idomList.size() <= 1 );
  if( idomList.size() == 1 ) {
    IntList::const_iterator it = idomList.begin();
    if( it != idomList.end() ) {
      retVal = *it;
    }
  }
  return retVal;
}

/**
 * Returns all variables in the graph
 */
void getAllVariables( DiGraph &graph,
                      IntMap &allVars ) {

  Vertex *vertex_p = 0;
 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end();
       it++ ) {
    vertex_p = *it;
    vertex_p->getAllVariables(allVars);
  }
}

/**
 * Returns the list of nodes that are children of the given node.
 * Children are the nodes that are successor of the given node
 * and the dominator of the successor is the given node.
 */
void getChildren( DiGraph &graph, 
                  int block,
                  const VertexListVector &idomList,
                  IntList &children ) {
 
  children.clear();
  int idom = 0;
  Vertex *vertex_p = 0;

  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end();
       it++ ) {
    vertex_p = *it;
    int num  = vertex_p->_block->num; 
    if( num != block ) {
      idom = getImmediateDom( idomList[num] );
      if( idom == block ) {
        if( ! Vertex::isItemInList( children, num) ) {
          children.push_back(num);
        }
      } 
    }
  }
}


/**
 * Computes the dominance frontier of each block
 */
void calculate_dominance_frontier( DiGraph &graph,
                                   int block,
                                   const VertexListVector &idomList,
                                   VertexListVector &dfList ) {
  Vertex *vertex_p = 0;
  IntList intList;
  IntList children;
  IntList::const_iterator listIt = 0;
  IntList::const_iterator dfIt = 0;
  int X = 0;
  int C = 0;

  int idom = 0;
  
  //for each C in children(B) do
  //childern of B is the successors in dominance tree.
  //We'll loop the successors and see if the successor in the
  //dominance list
  getChildren( graph, block, idomList, children);
  for( listIt = children.begin();
       listIt != children.end();
       listIt++ ) {

    calculate_dominance_frontier( graph, 
                                  *listIt,
                                  idomList, 
                                  dfList );
  }
  
  vertex_p = graph.getVertex(block);
  //DF(B) = Phi 
  dfList[block] = intList;
  //list is already initialized when calling this method.
  for( listIt = vertex_p->_adjList.begin();
      listIt != vertex_p->_adjList.end();
      listIt++ ) {
    X = *listIt;

    idom = getImmediateDom(idomList[X]);
    if( (-1 != idom) && (idom != block) ) {
      if(! Vertex::isItemInList( dfList[X], X )) {
        dfList[block].push_back(X);
      }
    } 
  }

  //for each C in children(B) do
  getChildren( graph, block, idomList, children);
  for( listIt = children.begin();
       listIt != children.end();
       listIt++ ) {
    C = *listIt;
    for( dfIt = dfList[C].begin();
       dfIt != dfList[C].end();
       dfIt++ ) {
      X = *dfIt;
      idom = getImmediateDom(idomList[X]);
      if( (-1 != idom) && (idom != block) ) {
        if(! Vertex::isItemInList( dfList[X], X )) {
            dfList[block].push_back(X);
        }
      } 
    }
  }
}
                                   
/**
 * Insert Phi functions
 */
void insertPhiFunctions( 
          DiGraph &graph,
          const VertexListVector &dfList,
          IntList &allDefs,
          IntMap &allVarsMap ) {

  //get all definitions
  IntListMap Aorig;
  IntListMap defsites;
  IntListMap Aphi;
  IntList list;
  Vertex *vertex_p;
  int block = 0;
  int var = 0;
  IntList::const_iterator listIt = 0;

  //initialize the maps
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
    
    block = vertex_p->_block->num;
    vertex_p->getDefinitions(list); 
    Aorig[block] = list;
    IntList emptyList;

    for( listIt = list.begin();
         listIt != list.end();
         listIt++ ) {
      var = *listIt;
      if( ! Vertex::isItemInList( allDefs, var) ) {
        allDefs.push_back(var);
      }
      IntList varList;
      if( defsites.find(var) != defsites.end() ) {
        varList = defsites[var];
      }
      if( ! Vertex::isItemInList(varList, block) ) {
        varList.push_back(block);
        defsites[var] = varList;
      }
    }
  }

  //end of init--------------------------------------
  IntList W;
  int a = 0; //variable
  int n = 0; //node
  int y = 0;
  IntList df;

  //for each variable a, W = defsites[a]
  for( listIt = allDefs.begin();
       listIt != allDefs.end();
       listIt++ ) {

    a = *listIt;
    W = defsites[a];  

    IntList emptyList;
    //intitialize the map to empy list
    Aphi[a] = emptyList;

    while( ! W.empty() ) {
      n = W.front();
      W.pop_front();

      df = dfList[n];

      for( IntList::const_iterator it = df.begin();
           it != df.end();
           it++ ) {
        y = *it;
        if( ! Vertex::isItemInList( Aphi[a], y ) ) {
          vertex_p = graph.getVertex(y);
          assert(vertex_p);
          vertex_p->addPhiInstr(a, allVarsMap[a]);
          
          IntList tempList = Aphi[a];
          tempList.push_back(y);
          Aphi[a] = tempList;

          if( ! Vertex::isItemInList( Aorig[y], a )) {
            W.push_back(y);
          }
        }
      }
    }
  }
}

/**
 * Returns the index of the given block in 
 * the list
 */
int getListIndex( const IntList &list,
                  int blockNum ) {

  int index = -1;
  int temp = 0;
  for( IntList::const_iterator it=list.begin();
       it != list.end();
       it++ ) {
    if( *it  == blockNum ) {
      index = temp;
      break;
    } 
    temp++;
  }
  return index;
} 


/**
 * Returns true if the variable is used
 */
bool isPhiVariableUsed( my_simple_reg *reg1,
                        my_simple_reg *reg2)  {
  bool status = false;

  if( (0 != reg1) && (0 != reg2) ) {
    if( reg1->num == reg2->num) {
    }
    if( (reg1->num == reg2->num) &&
        (reg1->subscript == reg2->subscript) ) {
      status = true;
    }
  }
  return status;
}

/**
 * Returns true, if the definition of phi function
 * is used in the graph
 */
bool isPhiDefinitionUsed( DiGraph & graph,
                          my_simple_reg *myReg ) {

  my_simple_instr *myInstr = 0;
  Vertex *vertex_p = 0;
 
  //search for the phi function
  //initialize the maps
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
 
    //go through all statements
    myInstr = vertex_p->_firstInstr;
 
    while( 0 != myInstr ) {
      //if( (myInstr->dst != 0 ) &&
      //    (myInstr->dst->reg != 0 ) && 
      //    (myInstr->dst->reg == myReg->reg ) ) {
        //if the instruction is the PHI instruction itself
        //where all these things started, then ignore and move on
      //} else {
        if( isPhiVariableUsed( myReg, myInstr->src1 )) {
          return true;
        } 
        if( isPhiVariableUsed( myReg, myInstr->src2 ) ) {
          return true;
        } 
        if( isPhiVariableUsed( myReg, myInstr->proc ) ) {
          return true;
        } 
        for( int i=0; i<myInstr->nargs; i++) {
          if( isPhiVariableUsed( myReg, myInstr->args[i] ) ) {
            return true;
          } 
        }
      //}
      myInstr = myInstr->next;
    }
  }
  return false;
}
 
/**
 * Minimizes the SSA
 */
void minimizeSSA( DiGraph &graph ) {

  my_simple_instr *myInstr = 0;
  my_simple_instr *tempInstr = 0;
  Vertex *vertex_p = 0;

  //search for the phi function
  //initialize the maps
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);

    //go through all statements
    myInstr = vertex_p->_firstInstr;

    while( 0 != myInstr ) {
      if( PHI_OP == myInstr->opcode ) {
        tempInstr = 0;

        if( ! isPhiDefinitionUsed( graph, myInstr->dst) ) {
          tempInstr = myInstr;
          //remove the unwanted PHI function
          if( 0!= myInstr->prev ) {
            myInstr->prev->next = myInstr->next;
          } 
          if( 0 != myInstr->next ) {
            //if the lablel is there, move the label down
            myInstr->next->lab = myInstr->lab;
            myInstr->next->prev = myInstr->prev;
          }
          if( myInstr == vertex_p->_firstInstr ) {
            vertex_p->_firstInstr = myInstr->next;
          }
          if( myInstr == vertex_p->_lastInstr ) {
            vertex_p->_lastInstr = 0;
          }
        }
      } 
      myInstr = myInstr->next;

      //delete the instruction removed
      if(tempInstr) {
        deleteMySimpleInstr(tempInstr);
      }
    }
  }
}

/**
 * Renames  the register with the new name
 */
void renameReg( my_simple_reg *myReg,
                IntStackMap &stackMap ) {
  if( myReg ) {
    int x = myReg->num;
    int i = stackMap[x].top();
    myReg->subscript = i;
  }
}

/**
 * Helper function for renaming variables
 */
void rename( DiGraph &graph,
             int blockNum,
             IntMap &Count,
             IntStackMap &StackMap,
             const VertexListVector &idomList ) {

  Vertex *vertex_p = graph.getVertex(blockNum);
  assert(vertex_p);

  my_simple_instr *myInstr = vertex_p->_firstInstr;
  int i = 0;
  int a = 0;
  my_simple_reg *myReg = 0;
  
  //for each statement S in  block n
  while( myInstr != 0 ) {
    if( PHI_OP != myInstr->opcode ) {
      //for each use of some variable x in S
      renameReg(myInstr->src1, StackMap ); 
      renameReg(myInstr->src2, StackMap ); 
      renameReg(myInstr->proc, StackMap ); 
      for(i=0; i<myInstr->nargs; i++) {
        renameReg( myInstr->args[i], StackMap );
      }
    }   
    //for each definition of some variable a in S
    if( myInstr->dst ) {
      a = myInstr->dst->num;
      Count[a] = Count[a] + 1;
      i = Count[a];
      StackMap[a].push(i);
      renameReg(myInstr->dst, StackMap );
    }
    myInstr = myInstr->next;
  }
  
  int Y = 0;
  int index = 0;
  Vertex *vertexY = 0;

  //for each successor Y of block n
  IntList::const_iterator it = 0;
  for( it=vertex_p->_adjList.begin();
       it != vertex_p->_adjList.end();
       it++ ) {
    Y = *it;
    vertexY = graph.getVertex(Y);
    assert(vertexY);

    //Suppose blockNum is indexth predecessor of Y
    index = getListIndex( vertexY->_prevList, blockNum );
    
    assert(index >=0);

    //for each phi function in Y
    my_simple_instr *myInstr = vertexY->_firstInstr;
    while( myInstr != 0 ) {
      if( PHI_OP == myInstr->opcode ) {
        assert( index < myInstr->nargs); //index should be less than the args array size
        myReg = myInstr->args[index];
        renameReg(myReg, StackMap );
      }
      myInstr = myInstr->next;
    }
  }

  //for each child X of n
  IntList children;
  getChildren( graph, blockNum, idomList, children );
  for( it=children.begin();
       it != children.end();
       it++ ) {
    rename(graph, *it, Count, StackMap, idomList);
  }

  //for each definition of some variable a in statment S in block n
  myInstr = vertex_p->_firstInstr;
  while( myInstr != 0 ) {
    if( myInstr->dst ) {
      a = myInstr->dst->num;
      StackMap[a].pop();
    }
    myInstr = myInstr->next;
  }
}
 
/**
 * Rename the variables
 */
void renameVariables( DiGraph &graph,
                      const IntMap &allVarsMap,
                      const VertexListVector &idomList ) {
  //intialize Count and Stack for each variable
  IntMap::const_iterator it = 0;
  IntStackMap StackMap;
  IntMap Count;

  int a = 0;

  for( it = allVarsMap.begin();
       it != allVarsMap.end(); 
       it++ ) {
    a = (*it).first; 
    Count[a] = 0;
    IntStack stack;
    stack.push(0);
    StackMap[a] = stack;
  }
  rename( graph,
          graph.getVertex(0)->_block->num,
          Count,
          StackMap,
          idomList );
} 

/**
 * Gets the minimal SSA
 */
void getMinimalSSA( DiGraph &graph,
                    const VertexListVector &dfList,
                    const VertexListVector &idomList ) {

  //inserting phi functions
  IntList allDefs;
  IntMap allVarsMap;
 
  getAllVariables(graph, allVarsMap);
 
  insertPhiFunctions(
              graph,
              dfList,
              allDefs,
              allVarsMap );
 
  //rename the variables
  renameVariables( graph, allVarsMap, idomList ); 
  minimizeSSA( graph );
 
  numberInstructions( graph );
}

/**
 * Returns the DU chain key for the given register
 */
std::string getDuChainKey( my_simple_reg *reg ) {
  assert(reg);

  std::string str;
  char c_num[33];
  sprintf( c_num, "%i", 10 );
  str.append( c_num );
  str.append("_");
  char c_subscript[33];
  sprintf( c_subscript, "%i", 10 );
  str.append( c_subscript );
  
  return str;
}

/**
 * returns the DU chain use for the given def
 */
void getDuChainUse( DiGraph &graph,
                    my_simple_reg *defReg,
                    DuChain &duChain,
                    DuChainVector &visited ) {

  my_simple_instr *myInstr = 0;
  my_simple_reg *reg = 0;
  Vertex *vertex_p = 0;
  int defRegNum = defReg->num;
  int defRegSubscript = defReg->subscript;

  assert( defReg);
 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
 
    //go through all statements
    myInstr = vertex_p->_firstInstr;
 
    while( 0 != myInstr ) {
      reg = 0; //initialize the reg
      //check the use in the src1 register
      if( (myInstr->src1) && 
          (myInstr->src1->num == defRegNum) &&
          (myInstr->src1->subscript == defRegSubscript) ) {
        reg = myInstr->src1;
      }else if( (myInstr->src2) &&
                (myInstr->src2->num == defRegNum) &&
                (myInstr->src2->subscript == defRegSubscript) ) {
        reg = myInstr->src2;
      }else if( (myInstr->proc) &&
                (myInstr->proc->num == defRegNum) &&
                (myInstr->proc->subscript == defRegSubscript) ) {
        reg = myInstr->proc;
      }else {
        if( myInstr->nargs > 0 ) {
          for(int i=0; i<myInstr->nargs;i++ ) {
            if( (myInstr->args[i]->num == defRegNum) &&
                (myInstr->args[i]->subscript == defRegSubscript ) ) {
              reg = myInstr->args[i];
              break;

            }
          }
        }
      } 
      if( 0 != reg ) {
        if( (PHI_OP != myInstr->opcode ) )  {
          duChain.addUse( myInstr->old_instruction_no);
        }else {
          if( ! isItemInDuChainList( visited, myInstr->dst ) ) {
            //call this function recurssively for the destination register of
            //the PHI function
            DuChain tempDuChain( myInstr->dst,duChain.getDef() );  
            visited.push_back(tempDuChain);
            getDuChainUse( graph, myInstr->dst, duChain, visited );
          }
        }
      }
      myInstr = myInstr->next;
    }
  }
}

/**
 * Method returns if the item is in DuChainList
 */
bool isItemInDuChainList( const DuChainVector &duChainList,
                          my_simple_reg *myReg ) {
  DuChainVector::const_iterator it = 0;
  assert(myReg);
  my_simple_reg *duReg = 0;

  for( it = duChainList.begin(); it != duChainList.end(); it++ ) {
    duReg = it->getReg();
    if( (duReg->num == myReg->num) &&
        (duReg->subscript == myReg->subscript) ) {
      return true;
    }
  }
  return false;
}

/**
 * Adds the DuChain
 */
void addDuChain( DiGraph &graph, 
                 int def,
                 my_simple_reg *myReg,
                 DuChainVector &duChainList ) {

  assert(myReg);
  DuChainVector visited;

  if( ! isItemInDuChainList( duChainList, myReg) ) {
    //not found, add new one
    //duchain class will make a copy of this register
    DuChain duChain( myReg, def );
    getDuChainUse(graph, myReg, duChain,visited );
    duChainList.push_back(duChain);
  } 
}

/**
 * Get the Vector of DU chains for the given graph
 */
void getDuChain( DiGraph &graph,
                 DuChainVector &duChainList ) {

  my_simple_instr *myInstr = 0;
  Vertex *vertex_p = 0;
 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
 
    //go through all statements
    myInstr = vertex_p->_firstInstr;
 
    while( 0 != myInstr ) {
      //check if this is a definition
      if( (PHI_OP != myInstr->opcode) &&
          (0 != myInstr->dst) &&
          ( REAL_REG != myInstr->dst->kind) ){
        addDuChain( graph, myInstr->old_instruction_no, myInstr->dst, duChainList);
      }
      myInstr = myInstr->next;
    }
  }

  //now look for the registers with 0 subscript
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
 
    //go through all statements
    myInstr = vertex_p->_firstInstr;

    while( 0 != myInstr ) {
      //check if any register has the subscript 0

      if( (myInstr->src1) &&
          ( myInstr->src1->num >= 0) && 
          ( myInstr->src1->kind != REAL_REG) && 
          (0 == myInstr->src1->subscript)) {
        addDuChain( graph, 0, myInstr->src1, duChainList);
      }
      if( (myInstr->src2) &&
          ( myInstr->src2->num >= 0) && 
          (0 == myInstr->src2->subscript) ) { 
        addDuChain( graph, 0, myInstr->src2, duChainList);
      } 
      if( (myInstr->proc) &&
          ( myInstr->proc->num >= 0) && 
          (0 == myInstr->proc->subscript) ) { 
        addDuChain( graph, 0, myInstr->proc, duChainList);
      } 
      if( myInstr->nargs > 0 ) {
        for(int i=0; i<myInstr->nargs;i++ ) {
          if( ( 0 == myInstr->args[i]->subscript) &&
              ( myInstr->args[i]->num >= 0) ) {
            addDuChain( graph, 0, myInstr->args[i], duChainList);
          }
        }
      }
      myInstr = myInstr->next;
    }
  }
}

/**
 * Prints the reg nos
 */
void printRegNumber( const my_simple_reg *myReg, FILE *fd ) {
  assert(myReg);
  switch( myReg->kind ) {
    case MACHINE_REG: {
      fprintf(fd, "m");
      break;
    }
    case TEMP_REG: {
      fprintf(fd, "t");
      break;
    }
    case PSEUDO_REG: {
      fprintf(fd, "r");
      break;
    }
    case SYMBOLIC_REG: {
      fprintf(fd, "s");
      break;
    }
    case REAL_REG: {
      fprintf(fd,"[r");
      break;
    }
    default: {
      fprintf(fd, "ERROR:unknown register kind");
    }
  }
  fprintf(fd, "%d", myReg->num);
  
  if( REAL_REG == myReg->kind ) {
    fprintf(fd,"]");
    if( myReg->disp > 0 ) {
      fprintf(fd,"+%d", myReg->disp);
    }
  }
}
 
/**
 * prints the DU chain
 */
void printDuChain( const DuChainVector &duChainList, FILE *fd ) {
  printHeader( "DU Chains", 0,fd );
  fprintf( fd, "Reg\tDef At\tUse At\n");
  std::string regType;
  my_simple_reg *myReg = 0;
  
  //convert the vector to list so that we can sort
  std::list<DuChain> duList;

  for( int i=0; i<duChainList.size(); i++ ) {
    duList.push_back(duChainList[i]);
  } 
  duList.sort();

  std::list<DuChain>::const_iterator it = 0;
  for( it = duList.begin(); it != duList.end(); it++ ) {
    DuChain chain = *it;
    myReg = chain.getReg();
    printRegNumber(myReg, fd);
    fprintf(fd, "\t");
    fprintf(fd, "%d", chain.getDef());
    fprintf(fd, "\t");

    IntSet set;
    convertListToSet(chain.getUseList(), set );
    printSet( set, fd, ",",  true );
    
    
    fprintf(fd, "\n");
    
  }
}

/**
 * prints the Webs
 */
void printWebs( const WebList &webList, FILE *fd ) {
  printHeader( "Webs", 0,fd );
  fprintf( fd, "Web\tReg\tDef At\t\tUse At\n");
  std::string regType;
  my_simple_reg *myReg = 0;
  
  WebList::const_iterator it = 0;
  for( it = webList.begin(); it != webList.end(); it++ ) {
    Web web = *it;
    myReg = web.getWebReg();
    printRegNumber(myReg, fd);
    fprintf(fd, "\t");
    myReg = web.getReg();
    printRegNumber(myReg, fd);
    fprintf(fd, "\t");

    IntSet set;
    convertListToSet(web.getDefList(), set );
    printSet( set, fd, ",",  true );
    fprintf(fd, "\t\t");
 
    set.clear();
    convertListToSet(web.getUseList(), set );
    printSet( set, fd, ",",  true );
 
    fprintf(fd, "\n");
  }
} 

/**
 * Returns true if the item is in web list
 */
bool isItemInWebList( const WebList &list,
                      const Web &web  ) {
  bool status = false;
  my_simple_reg *reg1 = 0;
  my_simple_reg *reg2 = 0;
 
  for( WebList::const_iterator it = list.begin();
    it != list.end();
    it++ ) {
 
    reg1 = it->getReg();
    assert(reg1);
    reg2 = web.getReg();
    assert(reg2);

    if( (reg1->num == reg2->num) &&
        (reg1->kind == reg2->kind) &&
        (reg1->subscript == reg2->subscript) ) {
      status = true;
      break;
    }
  }
  return status;
}


/**
 * Performs set minus on websets
 */
WebList webSetMinus( const WebList& s1, const WebList& s2 ) {
   WebList retSet;
   bool found = false;
   my_simple_reg* reg1 = 0;
   my_simple_reg* reg2 = 0;
   WebList::const_iterator it1;
   WebList::const_iterator it2;
  
   for( it1=s1.begin(); it1 != s1.end(); it1++ ) {
     reg1 = it1->getReg();
     assert(reg1);

     for( it2=s2.begin(); it2 != s2.end(); it2++ ) {
       reg2 = it2->getReg();
       assert(reg2);
       
       found = ((reg1->num == reg2->num) && 
                (reg1->subscript == reg2->subscript)) ? true: false;
       if(found) {
         break; //the inner loop
       }
     }
     if( !found ) {
       retSet.push_back(*it1);
     }
   }
   return retSet;
}


/**
 * Make the webs
 */
void makeWebs( const DuChainVector &duChainList,
               WebList &Webs ) {

  int oldnwebs = 0;
  int nwebs = 0;
  int i = 0;
  int nregs = 0;

  WebList::const_iterator it1 = 0;
  WebList::const_iterator it2 = 0;
  my_simple_reg *web1Reg = 0;
  my_simple_reg *web2Reg = 0;

  //first create 1 web for each du-chain
  for(i=0; i<duChainList.size(); i++ ) {
    DuChain duChain = duChainList[i];
    IntList defList;
    defList.push_back( duChain.getDef() );
    Web web(duChain.getReg(), defList, duChain.getUseList() );
    Webs.push_back(web);
    nwebs += 1;
  }

  do {
    //combine du-chains for the same symbol and that have a use in common
    // to make webs
    oldnwebs = nwebs;
    WebList Tmp1 = Webs;
    while( ! Tmp1.empty() ) {
      it1 = Tmp1.begin();
      Web web1 = *it1;
      WebList web1Set;
      web1Set.push_back(web1);
      Tmp1 = webSetMinus( Tmp1, web1Set);
      
      WebList Tmp2 = Tmp1;
      while( ! Tmp2.empty() ){
        it2 = Tmp2.begin();
        Web web2 = *it2;
        WebList web2Set;
        web2Set.push_back(web2);

        Tmp2 = webSetMinus( Tmp2, web2Set);

        web1Reg = web1.getReg();
        assert(web1Reg);
        web2Reg = web2.getReg();
        assert(web2Reg);

        IntList uses = setIntersection( web1.getUseList(),
                                        web2.getUseList() );

        if( (web1Reg->num == web2Reg->num) &&
            (! uses.empty()) ) {
          IntList defs =  setUnion(web1.getDefList(), 
                                   web2.getDefList());
          web1.setDefList(defs);
          uses = setUnion( web1.getUseList(),
                           web2.getUseList() );
          web1.setUseList(uses);

          //now modify the real Webs to put the new defs and uses
          WebList::iterator tempIt = 0;
          for(tempIt=Webs.begin(); tempIt != Webs.end(); tempIt++ ) {
            my_simple_reg *reg1 = tempIt->getReg();
            assert(reg1);
            my_simple_reg *reg2 = web1.getReg();
            assert(reg2);
            if( (reg1->num == reg2->num) &&
                (reg1->kind == reg2->kind) &&
                (reg1->subscript == reg2->subscript) ) {
              tempIt->setDefList(defs);
              tempIt->setUseList(uses);
            }
          }
          Webs = webSetMinus( Webs, web2Set);

          nwebs -= 1;
        }   
      }
    }
  }while( oldnwebs != nwebs );

  //assign symbolic register numbers to webs
  //sort the list for output formatting
  Webs.sort();
  i = nregs;
  for(it1=Webs.begin(); it1 != Webs.end(); it1++) {
    i += 1;
    web1Reg = it1->getWebReg();
    web1Reg->num = i;
  }
}

/**
 * Replaces the registers by webs
 */
void replaceWithWebs( DiGraph &graph,
                      const WebList &webs ) {

  Vertex *vertex_p = 0;
  //search for the phi function
  //initialize the maps
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
    vertex_p->replaceWithWebs(webs);
  }
}

/**
 * Returns the Out Bit vector
 */
void getOutBitVector( DiGraph &graph,
                     int currentNode,
                     BitVectorMap &inMap,
                     BitVector &bitVector ) {
 
  IntList succList = graph.getSuccessorList(currentNode);
  IntList::const_iterator it = 0;
  int count = 0;
 
  for( it = succList.begin(); it != succList.end(); it++ ) {
    if( 0 == count ) {
      bitVector = inMap[*it];
    } else {
      bitVector = bitVector + inMap[*it];
    }
    count++;
  }
}


/**
 * Computes the live variable analysis
 */
void liveVariables( DiGraph &graph,
                    BitVectorMap &defMap,
                    BitVectorMap &useMap,
                    BitVectorMap &inMap,
                    BitVectorMap &outMap,
                    unsigned int &iteration ) {

  iteration = 0; 
  int nodes = graph.size();
  if( nodes == 0 ) {
    return;
  }
  int vectorSize = defMap[0].size();
  BitVector oldIn(vectorSize);
  BitVector bitVector(vectorSize);

  bool change = true;

  while(change) {
    iteration++;

    change = false;
    for(int i =0; i<nodes;i++) {
      getOutBitVector(graph, i, inMap, bitVector);
      outMap[i] = bitVector;

      oldIn = inMap[i];
      inMap[i] = useMap[i] + ( outMap[i] - defMap[i] );

      if( ! ( inMap[i] == oldIn ) ) {
        change = true;
      }
    }
  }
}
 
/**
 * Performs the Live Variable Analysis
 */
void performLVA( DiGraph &graph,
                 const WebList &webs,
                 BitVectorMap &inMap,
                 BitVectorMap &outMap,
                 FILE *fd ) {

  WebList::const_iterator listIt = 0;
  BitVectorMap defMap;
  BitVectorMap useMap;
  int count = 0;    


  //initialize the bitvector
  if(fd ) {
    printHeader( "Live Variable Analysis", 0,fd );
    fprintf( fd, "Bit\tRegister\n");
    for( listIt=webs.begin(); listIt!=webs.end(); listIt++ ) {
      count++; 
      fprintf(fd, "%d\t", count);
      printRegNumber(listIt->getWebReg(), fd );
      fprintf(fd, "\n");
    }
  }

  //convert the graph to statement graph
  Vertex *vertex_p;
  DiGraph statementGraph;
  convertToStatementGraph(graph, statementGraph);
  inMap.clear();
  outMap.clear();

  BitVector bt(webs.size());

  if(fd ) {
    fprintf( fd, "\nStatement\tSuccessor\tDef\t\t\tUse\t\t\tIn\n");
  }

  count = 0;
  //loop through the graph to get the definition and use
  for( VertexVector::const_iterator it = statementGraph.begin();
       it != statementGraph.end();
       it ++ ) {

    inMap[count] = bt;
    outMap[count] = bt;
    defMap[count] = bt;
    useMap[count] = bt;

    vertex_p = *it;
    assert(vertex_p);
    vertex_p->getDefAndUse(defMap[count], useMap[count]);

    //donot print the first and last blocks
    if( (0 != fd ) && 
        (0 != count ) && 
        (count != statementGraph.size() - 1) ) {

      fprintf( fd, "%d\t\t", vertex_p->_block->num);
      IntList succList = vertex_p->_adjList;
      IntSet set;
      //remove the last block from the successor list
      if( count == statementGraph.size() - 2 ) {
        fprintf( fd, "None");
      }else {
        convertListToSet( succList, set );
        printSet( set, fd, ",", true );
      }
      fprintf( fd, "\t\t");
      defMap[count].print(fd, 4); 
      fprintf( fd, "\t\t");
      useMap[count].print(fd, 4); 
      fprintf( fd, "\t\t");
      inMap[count].print(fd, 4); 
      fprintf( fd, "\n");
    }
    count++;
  }

  unsigned int iter = 0;
  liveVariables( statementGraph, defMap, useMap, inMap, outMap, iter );

  if( fd ) {
    printHeader( 0, 0,fd );
    fprintf(fd, "After iteration %d\n", iter );
    fprintf( fd, "\nStatement\tOut\t\tIn\n");

    for( int i=0; i < inMap.size(); i++ ) {
      //donot print the first and last blocks
      if( (0 != i ) &&
          (i != inMap.size() - 1) ) {
        fprintf( fd, "%d\t", i);
        fprintf( fd, "\t");
        outMap[i].print(fd, 4);
        fprintf( fd, "\t\t");
        inMap[i].print(fd, 4);
        fprintf( fd, "\n");
      }
    }
  }
}

/**
 * Prints the coalescing
 */
void printCoalescing( IntPairVector regPairVector,
                      FILE *fd ) {
  printHeader( 0, 0,fd );
  fprintf(fd, "Coalescing:\t");
  if( regPairVector.size() == 0 ) {
    fprintf(fd, "None\n");
  }else {
    for(int i=0; i<regPairVector.size();i++) {
      IntPair regPair = regPairVector[i];
      if( 0 != i ) {
        fprintf(fd, "\t\t");
      }
      fprintf(fd, "s%d =\ts%d\n", regPair.first, regPair.second );
    }
  }
}

/**
 * returns true if the instruction is the register copy instruction
 */
bool isRegCopyInstr( my_simple_instr* myInstr ) {
  assert(myInstr);
  
  if( (CPY_OP == myInstr->opcode) &&
      ( 0 != myInstr->dst ) &&
      ( myInstr->dst->num >=0 ) &&
      ( 0 != myInstr->src1 ) &&
      ( myInstr->src1->num >=0 ) &&
      ( myInstr->src1->neg !=1 ) &&
      ( myInstr->src1->cvt !=1 ) &&
      ( myInstr->src1->not !=1 ) &&
      ( myInstr->oper == -1 ) &&
      ( 0 == myInstr->src2 ) &&
      ( 0 == myInstr->proc ) &&
      ( 0 == myInstr->nargs ) ) {
    return true;
  }
  return false;
} 

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
                          bool search ) {

  Vertex *vertex_p = graph.getVertex(blockNum);
  assert(vertex_p);

  bool found = false;
  if( 0 != vertex_p) {
    if( ! Vertex::isItemInList( visited, vertex_p->_block->num ) ) {
      visited.push_back(vertex_p->_block->num );
      if( search ) {
        found = isRegDefined( vertex_p->_firstInstr,
                              reg1, reg2 );
      }
      if( !found ) {
        //look in all the successors
        for(IntList::const_iterator it = vertex_p->_adjList.begin();
            it != vertex_p->_adjList.end();
            it++ ) {
          found = isRegDefinedInBlock( graph, *it, reg1, 
                                       reg2, visited, true );
          if( found ) {
            //break the loop if found
            break;
          }
        }
      }
    }
  }
  return found;
}
 
/**
 * This method starts searching for a definition of reg1/ reg2
 * starting from the instruction pointed by myInstr in the
 * current block. If it finds it, it will return true.
 */
bool isRegDefined( my_simple_instr *myInstr,
                   my_simple_reg *reg1, 
                   my_simple_reg *reg2 ) {

  assert(reg1);
  assert(reg2 );
  if( 0 != myInstr ) {
    my_simple_instr *curInstr = myInstr;
    my_simple_reg *dst = 0;
    while( 0 != curInstr ) {
      dst = curInstr->dst;
      if(0 != dst ) {
        if( (dst->num == reg1->num) ||
            (dst->num == reg2->num) ) {
          return true;
        }
      } 
      curInstr = curInstr->next; 
    }
  }
  return false;
} 

/**
 * Replaces the coalesced register 
 */
void replaceCoalescedReg( my_simple_reg *replaceReg, 
                          my_simple_reg *coalescedReg,
                          my_simple_reg *replaceWithReg ) {
  assert(coalescedReg);
  assert(replaceWithReg);

  if( replaceReg ) {
    if( (replaceReg->num == coalescedReg->num) &&
        (replaceReg->kind == coalescedReg->kind) ){
      replaceReg->num = replaceWithReg->num;
    }
  }
}

/**
 * Replaced the register with the coalesced register
 */
void replaceCoalescedRegInGraph( DiGraph &graph,
                                 my_simple_instr *regCopyInstr ) {

  assert(regCopyInstr);

  my_simple_reg *coalescedReg = regCopyInstr->src1;
  my_simple_reg *replaceWithReg = regCopyInstr->dst;

  assert(coalescedReg);
  assert(replaceWithReg);

  Vertex *vertex_p = 0;
  //loop through all statements to see if there is a register
  //copy statement
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
    my_simple_instr *myInstr = vertex_p->_firstInstr;
    while(0 != myInstr ) {
      if( myInstr != regCopyInstr ) {
        replaceCoalescedReg( myInstr->dst, coalescedReg, replaceWithReg );
        replaceCoalescedReg( myInstr->src1, coalescedReg, replaceWithReg );
        replaceCoalescedReg( myInstr->src2, coalescedReg, replaceWithReg );
        replaceCoalescedReg( myInstr->proc, coalescedReg, replaceWithReg );
        for(int i=0; i<myInstr->nargs; i++ ) {
          replaceCoalescedReg( myInstr->args[i], coalescedReg, replaceWithReg );
        }
      }
      myInstr = myInstr->next;     
    }
  }
}

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
                       IntPairVector &coalescePair ) {

  AdjMatrix *curMatrix = &adjMatrix;
  AdjMatrix *newMatrix = 0;
  
  Vertex *vertex_p = 0;
  bool coalesce = true;

  while( coalesce ) {
    coalesce = false; //initialize loop exiting condition
 
    //loop through all statements to see if there is a register
    //copy statement
    for( VertexVector::const_iterator it = graph.begin();
         it != graph.end(); it++ ) {

      vertex_p = *it;
      assert(vertex_p);
      my_simple_instr *myInstr = vertex_p->_firstInstr;
      while(0 != myInstr ) {
        if( isRegCopyInstr(myInstr) ) {
          int dst = myInstr->dst->num;
          int src = myInstr->src1->num;

          if(curMatrix->isSymRegInterfere( dst,src)) {
            //first seach in the current block starting from the next instruction
            //for the register definition
            bool found = isRegDefined( myInstr->next,     
                                       myInstr->dst,     
                                       myInstr->src1);     
 
            if( !found ) {
              //if not found then search in the sucessor list
               IntList visited;
               found = isRegDefinedInBlock( graph, vertex_p->_block->num,
                                            myInstr->dst, myInstr->src1,
                                            visited, false);
               visited.clear();
               coalesce = !found;
            }
          }else{
            coalesce = true;
          }
        }
        if( coalesce ) {
          IntPair regPair(myInstr->dst->num, myInstr->src1->num);
          coalescePair.push_back(regPair);

          //get the coalesced adjacency matrix
          newMatrix = curMatrix->getCoalescedMatrix(myInstr->src1,
                                                    myInstr->dst );
          if( curMatrix != &adjMatrix ) {
            delete curMatrix;
          }
          curMatrix = newMatrix;

          //replace src1 with dst
          replaceCoalescedRegInGraph(graph, myInstr ); 
          //delete the current instruction and break the loop
          vertex_p->deleteInstr( myInstr );
          myInstr = 0;
          break; //break the while loop
        }
        myInstr = myInstr->next;
      } //end while loop
      if( coalesce ) {
        break; //break the for loop
      }
    } //end for loop
  }
  return newMatrix;
}

/**
 * returns true if the instruction is the register copy instruction
 */
bool isRegImmediate( my_simple_instr* myInstr ) {
  assert(myInstr);
 
  if( ( 0 != myInstr->dst ) &&
      ( myInstr->dst->num >=0 ) &&
      ( 0 != myInstr->src1 ) &&
      ( myInstr->src1->is_immed ) &&
      ( myInstr->src1->neg !=1 ) &&
      ( myInstr->src1->cvt !=1 ) &&
      ( myInstr->src1->not !=1 ) &&
      ( myInstr->oper == -1 ) &&
      ( 0 == myInstr->src2 ) &&
      ( 0 == myInstr->proc ) &&
      ( 0 == myInstr->nargs ) ) {
    return true;
  }
  return false;
}

/**
 * Calculates the spill cost for the given instruction
 */
void spillCostReg( my_simple_reg *reg, 
                   int loopCount,
                   int unitCost,
                   IntMap &regMap ) {

  if( (reg) && (reg->num >= 0 ) ) {
    int cost = 0;
    int num = reg->num;
    if( regMap.find(num) != regMap.end() ) {
      cost = regMap[num];
    } 
    cost += (unitCost * loopCount);
    regMap[num] = cost;
  } 
}

/**
 * returns true if the symReg matches with the regnum
 */
bool isSymRegFound( int regNum,
                    my_simple_reg *myReg ) {
  if( (myReg) && (regNum == myReg->num)) {
    return true;
  }
  return false;
}

/**
 * returns true if the register is used anywhere in the graph
 */
bool isSymRegUsed( int regNum,
                   DiGraph &graph ) {

  Vertex *vertex_p = 0;
 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
 
    vertex_p = *it;
    assert(vertex_p);
    my_simple_instr *myInstr = vertex_p->_firstInstr;
    my_simple_reg *reg = 0;
    while(0 != myInstr ) {
      if( isSymRegFound(regNum, myInstr->src1)) return true;
      if( isSymRegFound(regNum, myInstr->src2)) return true;
      if( isSymRegFound(regNum, myInstr->proc)) return true;
      for( int i=0; i<myInstr->nargs;i++ ) {
        if( isSymRegFound(regNum, myInstr->args[i])) return true;
      }
      myInstr = myInstr->next;
    }
  }
  return false;
}

/**
 * Calculates the spill cost
 */
void calculateSpillCost( DiGraph& graph,
                         IntMap &regMap ) {

  Vertex *vertex_p = 0;
  int loopCount = 0;

  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
 
    vertex_p = *it;
    assert(vertex_p);
    loopCount = RegSpec::getInstance()->getLoopCount(
                                 vertex_p->_block->num );
    my_simple_instr *myInstr = vertex_p->_firstInstr;
    while(0 != myInstr ) {
      if(isRegImmediate( myInstr )) {
        spillCostReg( myInstr->dst,loopCount, 1, regMap ); 
      }else{
        spillCostReg( myInstr->dst,loopCount, 2, regMap ); 
        spillCostReg( myInstr->src1,loopCount, 2, regMap ); 
        spillCostReg( myInstr->src2,loopCount, 2, regMap ); 
        spillCostReg( myInstr->proc,loopCount, 2, regMap ); 
        for(int i=0; i<myInstr->nargs; i++ ) {
          spillCostReg( myInstr->args[i],loopCount, 2, regMap ); 
        }
      }
      myInstr = myInstr->next;
    }
  }
  //now go through the map to see if the registers are used in the 
  //program. If the registers are not used, then we will mark the
  //spill cost as -1, becuase they are dead variables.
  for(IntMap::iterator it = regMap.begin();
      it != regMap.end();
      it++ ) {
    if( !isSymRegUsed( it->first, graph )) {
      it->second = -1;
    }
  }
}

/**
 * Prints the spill cost
 */
void printSpillCost( IntMap &regMap,
                     FILE *fd ) {

  printHeader( "Spill Costs",0 , fd );
  int cost = 0;
  for(IntMap::const_iterator it = regMap.begin();
      it != regMap.end();
      it++ ) {
    cost = it->second;

    fprintf(fd, "s%d\t", it->first);
    if(-1 == cost ) {
      fprintf(fd,"pi");
    } else {
      fprintf(fd,"%d", cost);
    }
    fprintf(fd,"\n");
  }
}

/**
 * returns true, if the item is in the register stack
 */
bool isItemInRegStack( const MyRegStack &regStack, 
                       const my_simple_reg &reg ) {

  MyRegStack myStack = regStack;

  while( ! myStack.empty()) {
    my_simple_reg &myReg_r = myStack.top();
    if(AdjList::isRegEqual(myReg_r, reg) ) {
      return true;
    }
    myStack.pop();
  }

  return false;
}
 
/**
 * Prints register stack
 */
void printRegStack( const MyRegStack &regStack,
                    FILE *fd ) {

  MyRegStack myStack = regStack;
  printHeader( 0,0 , fd );
  fprintf( fd, "Register Stack %d\n\n", myStack.size());
  fprintf( fd, "top\t\t\tbottom\n");
  
  while( ! myStack.empty()) {
    const my_simple_reg &myReg_r = myStack.top();
    printRegNumber( &myReg_r, fd );
    fprintf( fd, "\t");
    myStack.pop();
  }
}

/**
 * move neighbors of i from adjnds to rmvadj and 
 * disconnect node i from its neighbors
 */
void adjustNeighbors( AdjListVector &adjVector,
                      int i ) {

  assert( (i>=0 && i<adjVector.size()) );
  
  MyRegList::const_iterator it = 0;

  //get the adjlist as reference
  MyRegList &srcadjnds_r = adjVector[i]._adjnds;
  const my_simple_reg &srcReg_r = adjVector[i]._reg;;

  //iterate through the list and remove this entry from the
  //target register lists
  for( it = srcadjnds_r.begin(); it != srcadjnds_r.end(); it++ ) {
    int pos = AdjList::getPos( adjVector, *it);
    assert(pos >= 0);
    adjVector[pos]._nints -= 1;
    AdjList::removeItemFromList( adjVector[pos]._adjnds, srcReg_r ); 

    adjVector[pos]._rmvadj.push_back(srcReg_r);
    adjVector[i]._rmvadj.push_back(*it);
  }
  
  adjVector[i]._nints = 0;
  adjVector[i]._adjnds.clear();
}

/**
 * Prunes the interference graph
 */
void pruneGraph( AdjListVector &adjVector, 
                 const WebList &webList,
                 MyRegStack &regStack,
                 FILE *fd  ) {
  int nregs = RegSpec::getInstance()->getNumMachineRegs();

  int nwebs = nregs + webList.size();
  int nodes = nwebs;
  bool success = false;
  int nints = 0;
  int i=0;
  float spillcost = 0.0;
  int stackNum = 0;
  int pos = 0;

  do {
    //apply degree < R rule and push nodes onto stack
    do {
      success = true;
      //it = webList.begin();

      for( i = 0; i<nwebs; i++ ) {
        nints = adjVector[i]._nints;
       
        if( nints < nregs ) {

         /* my_simple_reg reg;
          if(i < nregs ) {
            reg.kind = PSEUDO_REG;
            reg.num = i+1;
          }else {
            reg.kind = SYMBOLIC_REG;
            reg.num = it->getWebReg()->num;
            it++;
          }*/ 

          my_simple_reg reg = adjVector[i]._reg;
          if( !isItemInRegStack( regStack, reg) ) {
            success = false;
            //push the item to stack
            regStack.push(reg);
            //set the spill cost to inifinity
            pos = AdjList::getPos( adjVector, reg );
            assert(pos >= 0 );
            adjVector[pos]._spcost = FLT_MAX;
            adjustNeighbors(adjVector, i );
            nodes -=1;
            if(fd) {
              printRegStack( regStack ,fd );
              AdjList::print( adjVector, fd );
            }
          }
        }
      } 
    }while(false == success);

    if( nodes != 0 ) {
      if(fd) {
        printHeader( "Spill Priorities", 0, fd );
      }

      //find node with minimal spill cost divided by its degree and
      //push in onto stack (the optimistic heuristic)
      spillcost = FLT_MAX; 
      
      my_simple_reg spillnode;
      for( i = 0; i<nwebs; i++ ) {
        nints = adjVector[i]._nints; 
         
        float spill = adjVector[i]._spcost; 
        if( nints >0 ) {
          spill = spill/(float)nints;
        }
        if( spill < spillcost ) {
          spillnode = adjVector[i]._reg; 
          spillcost = spill;
        }
        //print the spill priorities
        if( fd ) {
          if( SYMBOLIC_REG == adjVector[i]._reg.kind ) {
            printRegNumber( &adjVector[i]._reg ,fd );
            fprintf(fd, "\t");
            if( FLT_MAX == spill) {
              fprintf(fd, "pi");
            } else {
              fprintf(fd, "%2.2f", spill);
            }
            fprintf(fd, "\n");
          }
        }
      }
      if( !isItemInRegStack( regStack, spillnode) ) {
        regStack.push(spillnode);

        pos = AdjList::getPos( adjVector, spillnode );
        assert(pos >= 0 );
        adjVector[pos]._spcost = FLT_MAX;
        adjustNeighbors(adjVector, pos );
        nodes -=1;
        
        if(fd) {
          printRegStack( regStack ,fd );
          AdjList::print( adjVector, fd );
        }
      }
    }
  }while( 0 != nodes ); 
}

/**
 * returns the min color for the adjList. If the color
 * cannot be assigned, it will return 0
 */
int getMinColor( const AdjListVector &adjVector,
                 my_simple_reg &reg ) {

  int nregs = RegSpec::getInstance()->getNumMachineRegs();
  int pos = AdjList::getPos( adjVector, reg );
  int color = 0;
  assert(pos >= 0);
  
  //initialize the machine register numbers
  IntSet colorSet;

  for( int i=1; i<=nregs;i++) {
    colorSet.insert(i); 
  }

  //now get all the adjnds and remove the color from the set
  //if they are already been assigned a color
  const MyRegList &regList_r = adjVector[pos]._rmvadj;

  for( MyRegList::const_iterator it = regList_r.begin();
       it != regList_r.end(); it++ ) {
    pos =  AdjList::getPos( adjVector, *it );
    assert(pos >= 0);
    
    color = adjVector[pos]._color;
    if( INT_MIN != color ) {
      colorSet.erase( color );      
    }
  }

  if( colorSet.size() > 0 ) {
    //get the first color
    IntSet::const_iterator setIt = colorSet.begin();
    color = *setIt; 
  }else{
    color = 0;
  }
  return color;
}

/**
 * Assign the registers
 */
bool assignRegs( AdjListVector &adjVector,
                 MyRegStack &regStack,
                 const IntList &alreadySpilledList,
                 IntList &spilledList,
                 FILE *fd ) {
  
  typedef std::map<int,MyRegList> MyRegListMap;
  bool success = true;
  int color = 0;
  int pos = 0;
  MyRegListMap myRegListMap;

  if(fd) {
    printHeader( "Asssigning Color", "Register\tColor", fd );
  }

  while(! regStack.empty() ) {
    my_simple_reg &myReg_r = regStack.top();
    regStack.pop();
    color = getMinColor( adjVector, myReg_r);
    pos =  AdjList::getPos( adjVector, myReg_r );
    assert(pos >= 0);

    if(fd) {
      printRegNumber( &(adjVector[pos]._reg), fd );
      fprintf(fd, "\t\t");
    }
    if( color > 0 ) {
      adjVector[pos]._color = color;
      if(fd) {
        fprintf(fd, "%d", color );
        MyRegList list;
        if( myRegListMap.find(color) != myRegListMap.end() ) {
          list = myRegListMap[color];
        }
        list.push_back(adjVector[pos]._reg);
        list.sort();
        myRegListMap[color] = list;
      }
    }else {
      //see if the item is already spilled
      if( ! Vertex::isItemInList( spilledList, myReg_r.num ) ) {
        adjVector[pos]._spill = true;
        spilledList.push_back( myReg_r.num );
      }else {
        if(fd){
          fprintf( fd, "Not spilling the register that already spilled" );
          printRegNumber( &myReg_r, fd );
          fprintf( fd, "\n");
        }
      }
      success = false;
      if(fd) {
        fprintf(fd, "spill" );
      }
    }    
    if(fd){
      fprintf(fd, "\n");
    }
  }

  //exit the program if we are trying to spill something, which we already spilled
  if( !success && (spilledList.size() == 0) ) {
    if(fd){
      fprintf( fd,  "\n%s %s %s\n\n", "No new register(s) spilled.",
                    "We are spilling already spilled registers only.",
                    "We cannot color this graph. Exiting ...");
    }
    exit(0);
  }

  if( fd && success ) {
    fprintf(fd, "\n");
    for(MyRegListMap::const_iterator mapIt = myRegListMap.begin();
        mapIt != myRegListMap.end(); mapIt++ ) {
      fprintf(fd, "m%d\t\t", mapIt->first);
      for(MyRegList::const_iterator listIt = mapIt->second.begin();
          listIt != mapIt->second.end(); listIt++ ) {
        printRegNumber( &(*listIt), fd );
        fprintf(fd, "\t");
      }
      fprintf(fd, "\n");
    }
  }
  return success;
}

/**
 * Modifies the code after register coloring
 */
void modifyCode( DiGraph &graph,
                 const AdjListVector &adjVector ) {
  Vertex *vertex_p = 0;
 
  for( VertexVector::const_iterator it = graph.begin();
       it != graph.end(); it++ ) {
    vertex_p = *it;
    assert(vertex_p);
    vertex_p->modifyCodeWithMachineReg( adjVector );
  }
}

/**
 * Generates the spill code and returns the displacement
 */
int genSpillCode( DiGraph &graph, 
                  const AdjListVector &adjVector, 
                  int lastdisp,
                  FILE *fd ) {

  const REAL_REG_NUM = 20;
  int disp = lastdisp;
  Vertex *vertex_p = 0;
  
  printHeader( "Spilled Code",0,fd );

  //loop through the adjvector to look for the spilled regs
  for( AdjListVector::const_iterator adjIt = adjVector.begin();
       adjIt != adjVector.end(); adjIt++ ) {
    if( adjIt->_spill ) {
      my_simple_reg *realReg = createMySimpleReg();
      realReg->kind = REAL_REG;
      realReg->num = REAL_REG_NUM;
      realReg->disp = disp;

      const my_simple_reg *symReg = &(adjIt->_reg);
      printRegNumber( symReg, fd );
      fprintf(fd, "->");
      printRegNumber(realReg, fd );
      fprintf(fd, "\n");

      for( VertexVector::const_iterator it = graph.begin();
           it != graph.end(); it++ ) {
        vertex_p = *it;
        assert(vertex_p);
        vertex_p->genSpillCode( symReg, realReg );
      }
      delete realReg;
      realReg = 0;

      disp += 4;
    }
  }
  fprintf(fd, "\n");
  //number the instuctions
  numberInstructions(graph);
  return disp;
} 

/**
 * Returns new web register list for the old spill list.
 */
IntList getSpilledList( const WebList &webList, 
                        const IntList spillList ) {

  IntList retList;
  int regNum = 0;

  for( IntList::const_iterator listIt = spillList.begin();
       listIt != spillList.end(); listIt++ ) {

    for( WebList::const_iterator it = webList.begin();
         it != webList.end(); it++ ) {
      if( *listIt == it->getReg()->num ) {
        regNum = it->getWebReg()->num;
        if( ! Vertex::isItemInList(retList, regNum) )  {
          retList.push_back( regNum );
        }
      }
    }
  }
  return retList;
}

/*
 * Allocate registers. This method repeats until, it can 
 * allocate the registers.
 */
void allocateRegisters( DiGraph &graph,
                        DuChainVector &duChainList,
                        FILE *fd ) {

  WebList *webList_p              = 0;
  BitVectorMap *inMap_p           = 0;
  BitVectorMap *outMap_p          = 0;
  AdjMatrix *matrix_p             = 0;
  DuChainVector *duChainList_p    = 0; 
  AdjMatrix *newMatrix_p          = 0;
  IntPairVector* regPairVector_p  = 0;
  IntMap *regMap_p                = 0;
  AdjList *adjList_p              = 0;
  AdjListVector *adjVector_p      = 0;
  MyRegStack *regStack_p          = 0;
  IntList spilledList;

  bool success                    = true;
  int count                       = 0;
  int regDisp                     = 0; //register displacement                       

  //--------------------------------------------------
  duChainList_p     = &duChainList; 
  
  do {
    //compute duchain if the list is not there

    if( count > 0 ) {
      DiGraph *ssaGraph_p = new DiGraph();
      *ssaGraph_p = graph;

      //compute Dominance Frontier
      Vertex *vertex_p = ssaGraph_p->getVertex( 0 );
      assert(vertex_p);
      int dominStartNode = vertex_p->_block->num;
 
      IntList emptyList;
      VertexListVector domin( ssaGraph_p->size(), emptyList);
      dom_comp(*ssaGraph_p,dominStartNode,domin );
      VertexListVector idomin( ssaGraph_p->size(), emptyList);
      idom_comp( *ssaGraph_p, domin, dominStartNode, idomin );
 
      VertexListVector dfList( ssaGraph_p->size(), emptyList);
      calculate_dominance_frontier( *ssaGraph_p,
                                    dominStartNode,
                                    idomin,
                                    dfList );
      numberOldInstructions( *ssaGraph_p );
      getMinimalSSA( *ssaGraph_p, dfList, idomin );
 
      duChainList_p = new DuChainVector();
      getDuChain(*ssaGraph_p, *duChainList_p );
      printDuChain( *duChainList_p, fd );
 
      delete ssaGraph_p;
    }
    
    //compute the webs
    webList_p = new WebList();
    makeWebs( *duChainList_p, *webList_p);
    printWebs( *webList_p, fd );

    replaceWithWebs( graph, *webList_p );
    printHeader( "After replacing by webs", 0, fd );
    printInstructions( fd, graph);

    //perform Live Variable analysis
    inMap_p = new BitVectorMap();
    outMap_p = new BitVectorMap();
    
    performLVA(graph, *webList_p, *inMap_p, *outMap_p, fd );

    //create adjacency matrix
    matrix_p = new AdjMatrix( *webList_p, *inMap_p, *outMap_p );
    if( count > 0 ) {
      matrix_p->buildSpillInterference();
    }else {
      matrix_p->build();
    }
    printAdjMatrix(*matrix_p, fd );
    //matrix_p->print(fd);

    //perform register coalescing
    regPairVector_p = new IntPairVector();
    newMatrix_p = coalesceRegister( graph, *matrix_p, *regPairVector_p );
    printCoalescing( *regPairVector_p, fd );
  
    if( 0 != regPairVector_p ) {
      delete regPairVector_p;
      regPairVector_p = 0;
    }

    if( count == 0 ) {
      fprintf(fd, "\nAfter register coalescing\n");
    }
    numberInstructions(graph);
    if( count == 0 ) {
      printInstructions( fd, graph);
    }
    if( 0 != newMatrix_p ) {
      printAdjMatrix(*newMatrix_p, fd );
      delete matrix_p;
      matrix_p = newMatrix_p;
      newMatrix_p = 0;
    }else {
      printAdjMatrix(*matrix_p, fd );
    }

    //calculate the spill cost
    regMap_p = new IntMap();
    calculateSpillCost( graph, *regMap_p );
    printSpillCost(*regMap_p, fd );

    //get the adjacency list
    adjList_p = new AdjList();
    adjVector_p = new AdjListVector();
    adjList_p->build( *matrix_p, *regMap_p,  *adjVector_p );
    adjList_p->print(*adjVector_p, fd );
 
    //prune the graph
    regStack_p = new MyRegStack();
    pruneGraph( *adjVector_p, matrix_p->getWebList(), *regStack_p, fd );

    if( count > 0 ) {
      spilledList = getSpilledList( *webList_p, spilledList );
    }
    IntList tempList;
    success = assignRegs( *adjVector_p, *regStack_p, spilledList, tempList, fd );
 
    spilledList = tempList;
    if(success) {
      modifyCode( graph, *adjVector_p);
      printHeader("Modified Code",0,fd );
    }else {
      regDisp = genSpillCode( graph, *adjVector_p, regDisp, fd );
    }
    printInstructions( fd, graph);

    //delete the newly created variables
    if( 0 != regStack_p) {
      delete regStack_p;
      regStack_p = 0;
    }
    if( 0 != adjVector_p) {
      delete adjVector_p;
      adjVector_p  = 0;
    }
    if( 0 != adjList_p) {
      delete adjList_p;
      adjList_p = 0;
    }
    if( 0 != regMap_p) {
      delete regMap_p;
      regMap_p = 0;
    }
    if( 0 != matrix_p ) {
      delete matrix_p;
      matrix_p = 0;
    }
    if( 0 != inMap_p) {
      delete inMap_p;
      inMap_p = 0;
    }
    if( 0 != outMap_p) {
      delete outMap_p;
      outMap_p = 0;
    }
    //if count is > 0, then delete the duchain
    if( (count > 0) && ( 0 != duChainList_p) ) {
      delete duChainList_p;
      duChainList_p = 0;
    } 
    if( 0 != webList_p) {
      delete webList_p;
      webList_p = 0;
    }
    count++;
    if ( count > 5 ) return;
  } while( false == success );
}

