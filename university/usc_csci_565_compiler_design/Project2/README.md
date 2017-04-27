
Project 1      

***********************************
Name: Sanjay Nair
id  : 61388
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
14. bitvector.h		# Bit Vector class
15. bitvector.cc	# Bit Vector class
16. expression.h	# Expression class
17. expression.h	# Expression class
18. mysimple.h	        # My Simple data structure
19. mysimple.cc	        # My Simple data structure
20. print.c		# Print routines
21. print.h		# Print routines
22. Makefile		# Modified: make file

Impelementation detail
-----------------------
The program first removes the temporary registers using instruction combining. Please note thatmany optimizations / implementations are possible to do instruction combining. At this point of time I've done instruction combining mostly for the test casesi. I've created an Expression class which encapsulates the Expression. Vertex class has the methods to find the gen and killi. MySimple.h file has the extended data structure which has new fields to support instruction combining. After calculating the gen and kill, the program does available expression. I have created a bitvector class to perform this operation. After performing the available expression, I create a statment graph from the block graph. The statment graph has one node for each statement.After the conversion, I perform the CSE algorithm to determine common sub-expression. Common subexpression algorithm looks its in map to see whether the expression has any predecessors. If yes, it will recurssively search the graph for matching expressions. After finding a unique set of predecessors, it applies the CSE algorithm. Currently, the algorithm just blindly does the u=a+b and a=u algorithm. Later, a copy propagation algorithm can remove unwanted copies of registers. 

Some outputs are not matching with the sample test cases provided. Most of them are just minor (only exception is CSE). 

Please refer to compile.txt for compilation and testing instructions.

