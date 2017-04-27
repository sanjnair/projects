
#if !defined __digraph_H__
#define __digraph_H__

#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include "vertex.h"

typedef std::map<int,int> IntMap;
typedef std::vector<int> IntVector;
typedef std::vector<bool> BoolVector;
typedef std::set<int> IntSet;
typedef std::vector<Vertex*> VertexVector;
typedef std::stack<int> IntStack;
typedef std::queue<int> IntQueue;
typedef std::vector<IntList> VertexListVector;

class DiGraph {

public:
	/**
	 * constructs a digraph
	 */
	DiGraph();

	/**
	 * destroys a digraph
	 */
	~DiGraph();

	/**
	 * Copy C'tor
	 */
	DiGraph(const DiGraph& rhs);

	/**
	 * Assignment Operator
	 */
	DiGraph& operator= (const DiGraph& rhs);

	/**
	 * Adds the vertex to graph. First vertex is the head
	 * of the basic block.
	 *
	 * Returns true if the addition is successful.
	 * 
	 */
	bool addVertex( struct basic_block *block );

	/**
	 * Adds the edge.
	 * Returns true if the addition is successful.
	 */
	bool addEdge( int fromVertex, int toVertex );

	/**
	 * Returns vertex for the id
	 */
	Vertex* getVertex( int vertexId );

	/**
	 * Returns true if the vertex exists for the given Id
	 */
	bool isVertexExist( int vertexId );

	/**
	 * Graph iterator
	 */
	VertexVector::const_iterator begin();

	/**
	 * Graph iterator
	 */
	VertexVector::const_iterator end();

	/**
	 * Returns the Vertex Vector
	 */
	IntList getVertexList(int startId, bool dfs = true);

	/**
	 * Returns the predecessor set of the vertex
	 *
	 */
	IntList getPredecessorList(int vertexId);

	/**
	 * Returns the predecessor set of the vertex
	 *
	 */
	IntList getSuccessorList(int vertexId);

	/**
	 * Depth first search algorithm on graph. This algorithm returns
	 * a set of nodes that can be reached from the node specified by 
	 * the start parameter.
	 */
	void depthFirstSearch( int start, IntVector& vct );

	/**
	 * Breadth first searches the graph
	 *
	 */
	void breadthFirstSearch( int start, IntVector& vct );


	/**
	 * Reverses the Graph.
	 */
	void reverse();

	/**
	 * Prints the graph
	 *
	 */
	void printGraph( FILE* fd );

	/** 
	 * returns the size of the graph
	 */
	inline int size() {
	   return _vertexVector.size();
	}

private:

	/**
	 * Depth first search algorithm on graph. This algorithm returns
	 * a set of nodes that can be reached from the node specified by 
	 * the start parameter.
	 */
	void depthFirstSearch_Aux( int start, BoolVector& unVisited, IntVector& vct );
	                 
	VertexVector _vertexVector;
	IntMap _vertexMap;		// map of vertex id vs internal id.
};

#endif //__digraph_H__
