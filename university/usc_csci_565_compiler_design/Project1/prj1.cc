#include <algorithm>
#include <stack>
#include <stdio.h>
#include <string>
#include "digraph.h"
#include "prjutil.h"
#include "prj1.h"


//variable for DFS_pp
IntVector pre;
IntVector post;
int m;
int n;
EdgeVector edgeVector;

/**
 * returns the set of items the node dominates
 */
void getDominatorSet( const VertexListVector& domin,
					  int vertexId,
					  IntSet& set ) {

	for(int i=0; i< domin.size(); i++ ) {
		if(isItemInList(domin[i], vertexId)) {
			set.insert(i);
		}
	}
}

/**
 * Print the dominator list
 */
void printDom1( DiGraph& graph, const VertexListVector& domin, FILE* fd ) {

	Vertex* vertex_p = 0;
	int vertexId;
	printHeader( "Dominators", "Block\tDominates", fd );

	for(int i=0; i< domin.size(); i++ ) {
		vertex_p = graph.getVertex( i);
		vertexId = vertex_p ->_block->num;

		fprintf( fd, "%d\t",vertexId );
		IntSet set;
		convertListToSet(domin[i], set );
		printSet( set, fd );
		printf("\n");
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

//--------------------------------------------------------------------------------
/*
 * Calculate the predecessor of each node
 */
void predecessor( DiGraph& graph,FILE* fd ){
    
	Vertex* vertex_p = 0;
	int blockNum = 0;
 
	printHeader( "Immediate Predecessor Set", "Block\tImmediate Predecessors", fd );
	for( VertexVector::const_iterator it = graph.begin();
	     it != graph.end();
		 it++ ) {
		vertex_p = *it;
		assert(vertex_p);
		blockNum = vertex_p->_block->num;
		fprintf( fd, "%d\t", blockNum);

		IntList list = graph.getPredecessorList(blockNum);
		IntSet set;
		convertListToSet(list, set);
		printSet( set, fd ); 
		fprintf(fd, "\n");
    }
}
 
/**
 * Calcuate successor of each node
 */
void successor( DiGraph& graph, FILE* fd ) {
 
    printHeader( "Immediate Successor Set", "Block\tImmediate Successors", fd );
    
	Vertex* vertex_p = 0;
	int blockNum = 0;

	for( VertexVector::const_iterator it = graph.begin();
	     it != graph.end();
		 it++ ) {
		vertex_p = *it;
		assert(vertex_p);
		blockNum = vertex_p->_block->num;
		fprintf( fd, "%d\t", blockNum);

		IntList list = graph.getSuccessorList(blockNum);
		IntSet set;
		convertListToSet(list,set);
		printSet( set, fd ); 
		fprintf(fd, "\n");
    }
}

/**
 * Computes the dominators.
 * 
 */
void pdom_comp( DiGraph& graph, int r, VertexListVector& domin ) {

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
            }
        }
    }while(change);
}

/**
 * Computes the dominators.
 * 
 */
void dom_comp( DiGraph& graph, int r, VertexListVector& domin ) {

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

		//for( IntSet::const_iterator s_it = Tmp[*it].begin();
		//	 s_it != Tmp[*it].end(); s_it++ ) {
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
 * Depth first search algorithm
 */
void depthFirstSearch_pp( DiGraph& graph, 
						  int x,
						  const VertexListVector& dominVector ) {

	IntSet::const_iterator it;
	Vertex* v = graph.getVertex(x);
	v->visited = true;
	
	pre[x] = n;
	n++;

	IntSet set;
	convertListToSet( graph.getSuccessorList(x),set);


	for( it = set.begin(); it != set.end(); it++ ) {
		v = graph.getVertex(*it);
		Edge edge;

		if( false == v->visited ) {
			depthFirstSearch_pp( graph, *it, dominVector );
			edge.startNode = x;
			edge.endNode = *it;
			edge.type = TREE; 
		} else if( pre[x] < pre[*it] ) {
			edge.startNode = x;
			edge.endNode = *it;
			//check for domin condition. for a forward edge,
			//tail should dominate head
			IntList forwardList = dominVector[edge.endNode];
			if(isItemInList( forwardList, edge.startNode ) ) {
				edge.type = FORWARD; 
			} else{
				edge.type = UNKNOWN; 
			}
		} else if( post[*it] == 0 ) {
			edge.startNode = x;
			edge.endNode = *it;

			//check for domin condition. for a back edge,
			//head should dominate head
			IntList backList = dominVector[edge.startNode];
			if(isItemInList( backList, edge.endNode ) ) {
				edge.type = BACK; 
			} else{
				edge.type = UNKNOWN; 
			}
		} else {
			edge.startNode = x;
			edge.endNode = *it;
			edge.type = CROSS; 
		}
		edgeVector.push_back( edge );
	}

	post[x] = m;
	m++;
}

/**
 * Print the edges information
 *
 */
void printEdges( EdgeVector& edgeVector, 
				EdgeType type,
				FILE* fd ) {

	std::string str;

	switch( type ) {
		case FORWARD:
			str = "Forward";
			break;
		case BACK:
			str = "Back";
			break;
		case CROSS:
			str = "Cross";
			break;
		default:
			return;				
	}

	str.append( " Edges" );
	
	printHeader( str.c_str(),0, fd );
	
	EdgeList list;
	for( int i= 0; i< edgeVector.size(); i++  ) {
		Edge edge = edgeVector[i];
		if(edge.type == type ) {
			list.push_back( edge );
		}
	}

	list.sort();
	int count = 1;
	if( list.size() > 0 ) {
		for( EdgeList::const_iterator listIt=list.begin();
			 listIt != list.end(); 
			 listIt++ ) {
			Edge edge = *listIt;
			fprintf( fd, "%d : %d -> %d", count, edge.startNode, edge.endNode );
			fprintf( fd, "\n");
			count++;
		}
	} else {
		fprintf( fd, "None");
	}
}

/**
 * calcuate edges
 *
 */
void calcuateEdges( DiGraph& graph, 
				    const VertexListVector& dominVector,
				    int startNode, 
					FILE* fd ) {

	m = 1;
	n = 1;
	int i = 0;
	
	pre.clear();
	post.clear();
	edgeVector.clear();
	Vertex* v = 0;

	for( i=0; i< graph.size(); i++ ) {
		pre.push_back(0);
		post.push_back(0);
		v = graph.getVertex(i);
		v->visited = false;
	}

	depthFirstSearch_pp( graph, startNode, dominVector );
	printEdges( edgeVector, FORWARD, fd );
	printEdges( edgeVector, BACK, fd );
	printEdges( edgeVector, CROSS, fd );
}

/**
 * Nat_loop insert 
 *
 */
void Nat_Insert( IntStack& stack, IntList& loop, int m ) {
	
	if( ! isItemInList(loop, m ) ) {
		IntList m_list;
		m_list.push_back(m);
		loop = setUnion( loop, m_list );
		m_list.push_back(m);
		stack.push(m);
	}
}

/**
 * Nat
 */
IntList Nat_Loop( DiGraph& graph,
				  int startNode,
				  int endNode ) {

	IntStack stack;
	IntList loop;
	int m;
	
	loop.push_back(endNode);
	Nat_Insert( stack, loop, startNode );

	while( ! stack.empty() ) {
		m = stack.top();
		stack.pop();
		
		IntList predList = graph.getPredecessorList( m );
		for( IntList::const_iterator it = predList.begin();
			 it != predList.end();
			 it++ ) {
			Nat_Insert( stack, loop, *it);
		}
	}
	return loop;
}

/**
 * Calculates the natural loop algorithm
 *
 */
void calculateNaturalLoops( DiGraph& graph,
						   const EdgeVector& edgeVector,
						   FILE* fd ) {

	printHeader( "Natural Loops",0, fd );
	int count = 1;
	for( int i=0; i< edgeVector.size(); i++ ) {
		Edge edge = edgeVector[i];

		if( BACK == edge.type ) {
			IntList list = Nat_Loop( graph, edge.startNode, edge.endNode );
			fprintf( fd, "%d : ", count );
			IntSet set;
			convertListToSet( list, set );
			printSet( set, fd, ",", true );
			fprintf( fd, "\n");
			count++;
		}
	}
	if( count == 1 ) {
		fprintf( fd, "None");
	}
}

/**
 * Executes the project 1 requirement
 *
 */
void execProject1( DiGraph& graph, FILE* fd ) {
	int dominStartNode = 0;
	int pdominStartNode = 0;
	Vertex* vertex_p = 0;
	
	IntList emptyList;

	// Predecessor & Successor
	predecessor( graph, fd );
    successor( graph, fd  );

	//dominator & immediate dominator
	vertex_p = graph.getVertex( 0 );
	dominStartNode = vertex_p->_block->num;

	VertexListVector domin( graph.size(), emptyList);
	dom_comp(graph,dominStartNode,domin ); 
	printDom( graph, domin, fd );
	//-------------------------------------------------------------

	DiGraph reverseGraph = graph;
	reverseGraph.reverse();
	//post dominator & immediate post immediate post dominator
	vertex_p = graph.getVertex( reverseGraph.size() - 1 );
	pdominStartNode = vertex_p->_block->num;

	VertexListVector pdomin(reverseGraph.size(), emptyList);
	pdom_comp(reverseGraph,pdominStartNode,pdomin ); 
	
	VertexListVector pidomin( reverseGraph.size(), emptyList);
	idom_comp( reverseGraph, pdomin, pdominStartNode, pidomin );
	printiDom( reverseGraph, pidomin, fd );

	calcuateEdges( graph, domin, dominStartNode, fd );
	calculateNaturalLoops( graph, edgeVector, fd );
}

/**
 * exectues the project 1 requirements.
 *
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

	execProject1( graph, fd );
}

