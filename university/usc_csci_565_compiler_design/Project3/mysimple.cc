////////////////////////////////////////////////////////////////
//MySimple.cc
////////////////////////////////////////////////////////////////
#include "mysimple.h"

/**
 * constructs a new my_simple_reg and initializes the variables
 */
my_simple_reg* createMySimpleReg( simple_reg *simpleReg) {
  my_simple_reg  *reg = new my_simple_reg;
  assert(reg);
  reg->is_immed = 0;
  reg->neg = 0;
  reg->cvt = 0;
  reg->not = 0;
  reg->index = 0;
  reg->subscript = -1;

  if(simpleReg) {
    reg->reg = simpleReg;
    reg->kind = simpleReg->kind;
    reg->num = simpleReg->num;
  }else {
    reg->reg = 0;
    reg->kind = -1;
    reg->num = -1;
  }

  return reg;  
}

/**
 * deletes the instruction from the given instruction.
 * users need to set their pointer to null after this.
 */
void deleteMySimpleReg( my_simple_reg *reg ) {
  assert(reg);
  delete reg;  
} 
 
/**
 * creates a copy of instruction from the given instruction.
 * users need to delete this copy after use.
 */
my_simple_reg* copyMySimpleReg( const my_simple_reg *from) {
  my_simple_reg *reg = 0;
  if( 0 != from ) {
    reg = createMySimpleReg();
    *reg = *from;
  }
  return reg;
} 

/**
 * Constructs a new my_simple_instr and initializes
 */
my_simple_instr* createMySimpleInstr() {

  my_simple_instr *instr = new my_simple_instr;
  assert(instr);
  instr->old_instruction_no =0;
  instr->new_instruction_no = 0;
  instr->block_no = 0;
  instr->next = 0;
  instr->prev = 0;
  instr->dst = 0;
  instr->src1 = 0;
  instr->src2 = 0;
  instr->target = 0;
  instr->instr_type = 0;
  instr->proc = 0;
  instr->nargs = 0;
  instr->args = 0;
  instr->lab = 0;
  instr->opcode = -1;
  instr->oper = -1;

  return instr;
}

/**
 * deletes the instruction from the given instruction.
 */
void deleteMySimpleInstr(my_simple_instr *from) {
  assert(from);
  if( from->dst ) {
    deleteMySimpleReg( from->dst );
  }
  if( from->src2 ) {
    deleteMySimpleReg( from->src1 );
  }
  if( from->src2 ) {
    deleteMySimpleReg( from->src2 );
  }

  if( from->proc ) {
    deleteMySimpleReg( from->proc );
  }
  if( from->nargs > 0 ) {
    for( int i=0; i<from->nargs; i++ ) {
      deleteMySimpleReg( from->args[i] );
    }
    delete [] from->args;
  }
  delete from;
}
 
/**
 * creates a copy of instruction from the given instruction.
 */
my_simple_instr* copyMySimpleInstr(const my_simple_instr *from) {
  my_simple_instr *instr = 0;
  if( 0 != from ) {
    instr = createMySimpleInstr();
    //just copy the values first, and then replace pointers 
    // with new memory
    *instr = *from;

    instr->dst = copyMySimpleReg(from->dst);
    instr->src1 = copyMySimpleReg(from->src1);
    instr->src2 = copyMySimpleReg(from->src2);
    instr->proc = copyMySimpleReg(from->proc);

    int nargs = from->nargs;
    if( nargs > 0 ) {
      instr->args = new my_simple_reg*[nargs];
      for(int n = 0; n < nargs; n++) {
        instr->args[n] = copyMySimpleReg(from->args[n]);
      }
    }

    instr->next = 0;
    instr->prev = 0;
  }
  return instr;
}

/**
 * Returns the opcode in string format given an opcode
 */
std::string getOpCodeStr( int opcode ) {
  std::string str ="";

  switch( opcode ) {
  case PHI_OP:
    str = "phi";
    break;
  case -1:
    break;
  default:
    simple_op_enum op = (simple_op_enum)opcode;
    str = simple_op_name(op); 
  }
  return str;
}

/**
 * returns true, if two registers are equal
 */
isEqual_reg( my_simple_reg *r1,
             my_simple_reg *r2 ) {

  bool status = ((0 != r1) && (0 != r2)) ? true : false;

  if(status) { 
    status = (r1->is_immed == r2->is_immed) ? true : false;
    if(status && r1->is_immed ) {
      status = isEqual_immed( r1->value, r2->value);   
    }
  }

  if(status){
    status = (r1->neg == r2->neg) ? true : false;
    if(status) {
      status = (r1->cvt == r2->cvt) ? true : false;
    }
    if(status) {
      status = (r1->not == r2->not) ? true : false;
    }
    if(status) {
      status = (r1->index == r2->index) ? true : false;
    }
  }

  if(status){
    //check if the register kind and num matches

    status = ( (r1->kind == r2->kind) && (r1->num == r2->num) ) ? true : false;

    /* 
    if( (0 != r1->reg) && (0 != r2->reg) ) {
      status = (r1->reg->kind == r2->reg->kind) ? true : false;
      if(status) {
        status = (r1->reg->num == r2->reg->num) ? true : false;
      }
    } */
    
  }

  return status;
}

/**
 * returns true, if two immed are equal 
 */
isEqual_immed( simple_immed &i1, simple_immed &i2 ) {
  bool status = false;

  status = (i1.format == i2.format) ? true : false;

  if( status ) {
    switch( i1.format) {
      case IMMED_INT:
        status = (i1.u.ival == i2.u.ival) ? true: false;
        break;
      case IMMED_FLOAT:
        status = (i1.u.fval = i2.u.fval) ? true : false;
        break;
      case IMMED_SYMBOL:
         status = (i1.u.s.symbol == i2.u.s.symbol) ? true : false;
         if(status){
           status = (i1.u.s.offset == i2.u.s.offset) ? true : false;
         }
    }
  }
  
  return status;
}
