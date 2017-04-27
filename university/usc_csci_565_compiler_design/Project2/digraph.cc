#include "digraph.h"

/**
 * constructs a digraph
 */
DiGraph::DiGraph() { }

/**
 * destroys a digraph
 */
DiGraph::~DiGraph() {

	//delete all the vertexes
	for( VertexVector::iterator it = _vertexVector.begin();
		 it != _vertexVector.end();
		 it++ ) {
		delete *it;
	}
}

/**
 * Copy C'tor
 */
DiGraph::DiGraph(const DiGraph& rhs) {
	Vertex* vertex_rhs_p = 0;
	Vertex* vertex_p = 0;

	for( int i =0; i< rhs._vertexVector.size(); i++ ) {
		vertex_rhs_p = rhs._vertexVector[i];
		vertex_p  = new Vertex( vertex_rhs_p->_block);
		vertex_p->_adjList = vertex_rhs_p->_adjList;
		vertex_p->_prevList = vertex_rhs_p->_prevList;
		_vertexVector.push_back(vertex_p);
	}

	_vertexMap = rhs._vertexMap;
}

/**
 * Assignment Operator
 */
DiGraph& DiGraph::operator= (const DiGraph& rhs) {
	Vertex* vertex_rhs_p = 0;
	Vertex* vertex_p = 0;

   //TODO delete the memory of previous items

	for( int i =0; i< rhs._vertexVector.size(); i++ ) {
		vertex_rhs_p = rhs._vertexVector[i];
		vertex_p  = new Vertex( vertex_rhs_p->_block );
		vertex_p->_adjList = vertex_rhs_p->_adjList;
		vertex_p->_prevList = vertex_rhs_p->_prevList;
		_vertexVector.push_back(vertex_p);
	}

	_vertexMap = rhs._vertexMap;

	return *this;
}


/**
 * Adds the vertex to graph. First vertex is the head
 * of the basic block.
 *
 * Returns true if the addition is successful.
 * 
 */
bool DiGraph::addVertex( struct basic_block *block ) {
	bool status = false;
	assert(block);

	int vertexId = block->num;
	if( ! isVertexExist(vertexId) ){
		Vertex *vertex = new Vertex(block);
		_vertexVector.push_back(vertex);
		_vertexMap[vertexId] = (_vertexVector.size() - 1);
	}
	return status;
}

/**
 * Adds the edge.
 * Returns true if the addition is successful.
 */
bool DiGraph::addEdge( int fromVertex, int toVertex ) {
	bool status = false;

	if( isVertexExist( fromVertex ) &&
		isVertexExist( toVertex ) ) {
		
		Vertex* v = getVertex( fromVertex );
		v->_adjList.push_back( toVertex );

		v = getVertex( toVertex );
		v->_prevList.push_back( fromVertex );
	}
	return status;
}

/**
 * Returns true if the vertex exists for the given Id
 */
bool DiGraph::isVertexExist( int vertexId ){
	return ( (_vertexMap.find(vertexId) != _vertexMap.end() ) ? true : false );
}

/**
 * Returns vertex for the id
 */
Vertex* DiGraph::getVertex( int vertexId ) {
	Vertex* vertex_p = 0;

	if( isVertexExist( vertexId ) ) {
		int id = _vertexMap[vertexId];
		vertex_p = _vertexVector[id];
	}
	return vertex_p;
}

/**
 * Graph iterator
 */
VertexVector::const_iterator DiGraph::begin() {
	return (_vertexVector.begin());
}

/**
 * Graph iterator
 */
VertexVector::const_iterator DiGraph::end() {
	return (_vertexVector.end());
}


/**
 * Returns the vertex set
 * @param dfs   depth first search
 */
IntList DiGraph::getVertexList(int startId,  bool dfs ) {
   IntList list;
   Vertex* vertex_p;
   int id = 0;

   if( ! dfs ) {
	   for( VertexVector::const_iterator it = _vertexVector.begin();
			it != _vertexVector.end();
			it++ ) {
       
			vertex_p = *it;
			id = vertex_p->_block->num;
			list.push_back(id);
	   }
   } else {
		IntVector vct;
		depthFirstSearch( startId, vct );

		for( IntVector::const_iterator it1 = vct.begin();
			it1 != vct.end();
			it1++ ) {
			id = *it1;
			list.push_back(id);
		}
   }
   return list;
}

/**
 * Returns the predecessor set of the vertex
 *
 */
IntList DiGraph::getPredecessorList( int vertexId ) {
    IntList list;

	Vertex *vertex_p = getVertex(vertexId);

    if( vertex_p ) {
        list = vertex_p->_prevList;
    }
    return list;
}

/**
 * Returns the predecessor set of the vertex
 *
 */
IntList DiGraph::getSuccessorList( int vertexId) {
	IntList list;
    Vertex *vertex_p = getVertex(vertexId);
    if( vertex_p ) {
        list = vertex_p->_adjList;
    }
    return list;
}

/**
 * Depth first search algorithm on graph. This algorithm returns
 * a set of nodes that can be reached from the node specified by 
 * the start parameter.
 */
void DiGraph::depthFirstSearch( int start, IntVector& vct ) {
	BoolVector visitVector( _vertexVector.size(), true );
	depthFirstSearch_Aux( start, visitVector, vct );
}

/**
 * Depth first search algorithm on graph. This algorithm returns
 * a set of nodes that can be reached from the node specified by 
 * the start parameter.
 */
void DiGraph::depthFirstSearch_Aux( int start, BoolVector& unVisited, IntVector& vct ) {

	if( start >= _vertexVector.size() ) {
		return;
	}

	vct.push_back(start);
	unVisited[start] = false;

	IntList::const_iterator it = 0;

	//traverse the adjacency list performing the depth first search
	//from eacg unvisited vertex in it.
	Vertex* vertex_p = _vertexVector[start];

	assert(vertex_p);

	for( it = vertex_p->_adjList.begin();
	     it != vertex_p->_adjList.end();
		 it++ ) {

		int xx = *it;
		//check if the current vertex has been visited
		 if( unVisited[*it] ) {
			//start the DFS from the new node
			depthFirstSearch_Aux( *it, unVisited, vct );
		 }
	}
}

/**
 * Breadth first searches the graph
 *
 */
void DiGraph::breadthFirstSearch( int start, IntVector& vct ) {
	BoolVector unVisited( _vertexVector.size(), true );

	IntQueue queue;
	Vertex* vertex_p = 0;
	unVisited[start] = false;
	IntList::const_iterator it = 0;

	queue.push( start );
	vct.push_back(start);

	while( ! queue.empty() ) {
		int id = queue.front();
		queue.pop();

		vertex_p = _vertexVector[id];
		for( it = vertex_p->_adjList.begin();
			 it != vertex_p->_adjList.end();
			 it++ ) {
			//check if the current vertex has been visited
			if( unVisited[*it] ) {
				unVisited[*it] = false;
				queue.push( *it );
				vct.push_back(*it);
			}
		}
	}
}

/**
 * Reverses the Graph.
 */
void DiGraph::reverse() {

	VertexVector::const_iterator it= 0;
	Vertex* vertex_p = 0;
	
	for( it = _vertexVector.begin();
	     it != _vertexVector.end();
		 it++ ) {
		
		vertex_p = *it;
		assert(vertex_p);
		IntList tempList = vertex_p->_adjList;
		vertex_p->_adjList = vertex_p->_prevList;
		vertex_p->_prevList = tempList;
	}
}


/**
 * Prints the graph
 *
 */
void DiGraph::printGraph( FILE* fd ) {
	
	Vertex* vertex_p = 0;
	int vertexId = 0;
	IntList::const_iterator it;

	for(int i=0; i< _vertexVector.size(); i++ ) {
		vertex_p = _vertexVector[i];
		vertexId = vertex_p->_block->num;
		fprintf( fd, "[%d]\n", vertexId);

		for( it = vertex_p ->_adjList.begin();
		     it != vertex_p ->_adjList.end();
			 it++ ) {
			
			fprintf( fd, "%d", vertexId);
			fprintf( fd, " -> ");
			fprintf( fd, "%d", *it);
			fprintf( fd, "\n");
		}

		for( it = vertex_p ->_prevList.begin();
		     it != vertex_p ->_prevList.end();
			 it++ ) {
			
			fprintf( fd, "%d", vertexId);
			fprintf( fd, " <- ");
			fprintf( fd, "%d", *it);
			fprintf( fd, "\n");
		}

		fprintf( fd, "\n");
	}
}
