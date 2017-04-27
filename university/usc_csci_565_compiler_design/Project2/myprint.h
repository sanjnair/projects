////////////////////////////////////////////////////////////////
//myprint.h
////////////////////////////////////////////////////////////////
#if !defined __MYPRINT_H__
#define __MYPRINT_H__

/**
 * Constructs a new my_simple_instr and initializes
 */
extern my_simple_instr* createMySimpleInstr();

/** 
 * Prints the instruction
 */
extern void fprint_myInstr( FILE *fd, my_simple_instr *s );

/**
 * Prints register
 */ 
extern void fprint_myReg( FILE *fd, my_simple_reg *r );

/**
 * Prints the Expression
 */
extern void fprint_expression( FILE *fd, Expression &exp);

/**
 * returns true, if two registers are equal
 */
extern isEqual_reg( my_simple_reg *r1, my_simple_reg *r2 ); 

/**
 * returns true, if two immed are equal
 */
extern isEqual_immed( simple_immed &i1, simple_immed &i2 ); 

#endif //__MYPRINT_H__

