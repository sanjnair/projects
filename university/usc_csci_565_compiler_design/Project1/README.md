
Project 1      

***********************************
Name: Sanjay Nair
***********************************

Files Included 
--------------
1. bb1.c 		# Modified : Basic Block
2. bb1.h		# Modified : Basic Block
3. digraph.cc		# New: Direct graph 
4. digraph.h		# New: Direct graph 
5. prj1.cc		# New: Project 1 implementation
6. prj1.h		# New: Project 1 implementation
7. prjutil.cc 		# New: Project utilities
8. prjutil.h		# New: Project utilities
9. vertex.cc		# New: Vertex class
10. vertex.h		# New: Vertex class
11. print.c		# Not modified
12. print.h		# Not modified
13. main.c		# Not modified
14. Makefile		# Modified: make file

Impelementation detail
-----------------------
I have created a class named digraph, which encapsulates a directed graph. 
The digraph class keeps a list of vertexes, which has the information
about individual vertex. Most of the program is written in c++, heavily
using the STL container classes. Each vertex stores the adjacent list 
as well as previous list of the vertexes. 

Most of the project implementation is done in prj1.cc file. prj1.cc 
uses the digraph to compute the algorithms asked in project1. prjutil has 
set operation methods and other utility methods. 

The project1 first builds a directed graph and then computes predecessor
and successor of each nodes. It then uses this information to compute
dominator for each node. Sequential numbering of nodes is assumed here,
because suif compiler always returns the blocks in sequential number 
starting from 0. Impelentation can be easily changed to accommodate non- 
sequential numbers or even names by maintainig two separate maps 
(between internal id and block id / name ). After computing the dominators,
it reverses the graph (i.e all the predecessor list and successor list is
swapped for each vertex). The program runs the dominator algorithm on the
reversed graph and then runs immediate dominator algorithm to produce 
the immediate post dominator set for each node.

The dominator function gets the list of "reachable" nodes to perform the
dominator algorithm. This is done by searching the graph in depth-first
order. 

The program then calculates edges using the depth first search tree. 
It then classifies the edges as TREE, FORWARD, BACK and CROSS. 
Additional check is been  made to make sure that the FORWARD and BACK edges
satisfies the dominator requirement.  Finally, the program calculates the 
natural loop for each BACK edges. All the results are printed on the console
window(shell).

Please refer to compile.txt for compilation and testing instructions.

