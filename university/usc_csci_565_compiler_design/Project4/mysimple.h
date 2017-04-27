////////////////////////////////////////////////////////////////
//MySimple.h
////////////////////////////////////////////////////////////////

#if !defined __MYSIMPLE_H__
#define __MYSIMPLE_H__

#include <string>
extern "C" 
{
  #include "simple.h"
}


/////////////////////////////////
//Extention of register kind
/////////////////////////////////
typedef enum my_reg_kind_enum {
  SYMBOLIC_REG = 100,
  REAL_REG = 101
}my_reg_kind;

/////////////////////////////////
//Extention of opcode kind
/////////////////////////////////
typedef enum my_opcode_enum {
  PHI_OP = 200,
  SAV_OP = 201
}my_op_code;

/////////////////////////////////
//Extention of simple_reg struct
/////////////////////////////////
typedef struct my_simple_reg_struct {
  int is_immed;            // If true, second item is the immediate value 
  simple_immed value;      //
  int neg;                 // unary operator neg
  int cvt;                 // unary convert operator
  int not;                 // unary not operator       
  int index;               // index
  int subscript;           // subscript
  int kind;                // register kind
  int num;                 // register number
  int disp;                // register displacment
  simple_reg *reg;         // pointer to original reg structure

  /**
   * overloaded < operator for sorting
   */
  bool operator<(const my_simple_reg_struct& y) {
    if( kind != y.kind ) {
      return (kind < y.kind);
    }
    return num  < y.num;
  }
} my_simple_reg;

/////////////////////////////////
//Extention of simple_instr struct
/////////////////////////////////
typedef struct my_simple_instr_struct {
  int old_instruction_no;  // old instruction number (for combining)
  int new_instruction_no;  // New instruction number
  int block_no;            // Block number

  int opcode;              // opcode 
  int oper;                // operator 

  struct my_simple_instr_struct *next; //ptr to next instruction
  struct my_simple_instr_struct *prev; //ptr to previous instruction

  my_simple_reg *dst;      // destination register
  my_simple_reg *src1;     // source 1 register
  my_simple_reg *src2;     // source 2 register

  simple_sym *target;      // branch target label
  int instr_type;          // 0 for base, 1 for bj  

  simple_immed value;      //immediate constant

  my_simple_reg *proc;     // Address of the callee
  unsigned int nargs;      // number of arguments
  my_simple_reg **args;    // array of arguments

  simple_sym *lab;         // the symbol for this label
} my_simple_instr;

/**
 * Constructs a new my_simple_instr and initializes
 */
extern my_simple_instr* createMySimpleInstr();

/**
 * deletes the instruction from the given instruction.
 */
extern void deleteMySimpleInstr( my_simple_instr *from);

/**
 * creates a copy of instruction from the given instruction.
 */
extern my_simple_instr* copyMySimpleInstr( const my_simple_instr *from);

/**
 * constructs a new my_simple_reg and initializes the variables
 */
extern my_simple_reg* createMySimpleReg(simple_reg *simple_Reg = 0 );

/**
 * deletes the instruction from the given instruction.
 */
extern void deleteMySimpleReg(my_simple_reg *from);

/**
 * creates a copy of instruction from the given instruction.
 */
extern my_simple_reg* copyMySimpleReg( const my_simple_reg *from);

/**
 * Returns the opcode in string format given an opcode
 */
extern std::string getOpCodeStr( int opcode );

/**
 * returns true, if two registers are equal
 */
extern isEqual_reg( my_simple_reg *r1, my_simple_reg *r2 ); 

/**
 * returns true, if two immed are equal
 */
extern isEqual_immed( simple_immed &i1, simple_immed &i2 ); 

#endif //__MYSIMPLE_H__
