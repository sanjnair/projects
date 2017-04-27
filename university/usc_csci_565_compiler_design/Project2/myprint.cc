////////////////////////////////////////////////////////////////
//MyPrint .cc
////////////////////////////////////////////////////////////////

#include <string>
#include <stdio.h>

extern "C" {
  #include "simple.h"
  #include "print.h"
}

#include "mysimple.h"
#include "expression.h"

/**
 * Prints register
 */
void fprint_myReg( FILE *fd, my_simple_reg *r ) {
  assert( r);
 
  if( r->cvt ) {
    fprintf( fd, "cvt " );
  }else if( r->neg ) {
    fprintf( fd, "neg " );
  }else if( r->not ) {
    fprintf( fd, "not " );
  }
 
  if( 0 != r->reg ) {
    fprint_reg( fd, r->reg );
  } else {
    fprint_immed( fd, &(r->value) );
  }
}

/** 
 * Prints the instruction
 */
void fprint_myInstr( FILE *fd, my_simple_instr *s ) {
  assert(s);

  if( 0 == s->old_instruction_no ) {
    fprintf(fd, "[%2s/%2d/%2d]:",
               "xx",
               s->new_instruction_no,
               s->block_no );
  } else {
    fprintf(fd, "[%2d/%2d/%2d]:",
               s->old_instruction_no,
               s->new_instruction_no,
               s->block_no );
  }

  fprintf(fd, "  ");
  if( s->lab) {
    fprintf(fd,"%s:", s->lab->name);
  }else {
    fprintf(fd, "   " );
  } 
  fprintf(fd, "\t");
  if( LABEL_OP != s->opcode) {
    fprintf(fd, "%s ", (getOpCodeStr(s->opcode)).c_str());
  }

  fprintf(fd, "\t");
  if( s->dst ) {
    fprint_myReg( fd, s->dst);
    fprintf(fd, " = ");
  }
  fprintf(fd, "\t");
  if( s->src1 ) {
    fprint_myReg( fd, s->src1);
  }

  fprintf(fd, "\t");
  if( s->proc ) {
    fprintf(fd, "*");
    fprint_myReg( fd, s->proc);
  }

  //print the list of arguments
  int nargs = s->nargs;
  if (nargs != 0) {
    fprintf(fd, " (");
    for(int n = 0; n < nargs - 1; n++) {
      fprint_myReg(fd, s->args[n]);
      fprintf(fd, ", ");
    }
    fprint_myReg(fd, s->args[nargs - 1]);
    fprintf(fd, " )");
  }
 
  fprintf(fd, "\t");
  if( LABEL_OP != s->opcode) {
    fprintf(fd, "%s ", (getOpCodeStr( s->oper)).c_str());
  }
  
  fprintf(fd, "\t");
  if( s->src2 ) {
    fprint_myReg( fd, s->src2);
  }

  fprintf(fd, "\t\t");
  if( s->target) {
    fprintf(fd, "%s", s->target->name);
  } 
  fprintf(fd, "\n");
}

/**
 * Prints the Expression
 */
void fprint_expression( FILE *fd, Expression &exp ) {
  assert( exp._src1);
  fprint_myReg(fd, exp._src1);
  
  if(-1 != exp._oper) {
    fprintf(fd, "\t");
    fprintf(fd, "%s ", (getOpCodeStr(exp._oper)).c_str());
  }

  if( 0 != exp._src2) {
    fprintf(fd, "\t");
    fprint_myReg(fd, exp._src2);
  } 
}

