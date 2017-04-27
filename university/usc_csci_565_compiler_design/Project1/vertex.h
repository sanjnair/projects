
#if !defined __vertex_H__
#define __vertex _H__

#include <list>
#include <vector>
#include "bb1.h"

typedef std::list<int> IntList;


typedef enum {
	TREE, FORWARD, BACK, CROSS, UNKNOWN
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


class Vertex {

public:
	
	/** 
	 * Constructs a Vertex with id and block
	 */
	Vertex( struct basic_block *block );

	IntList _adjList; // adjacency list
	IntList _prevList; // previous node list
	struct basic_block *_block;
	bool visited;
};

#endif //__vertex _H__
