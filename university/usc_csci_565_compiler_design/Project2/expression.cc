#include "expression.h"
#include <stdio.h>
#include "mysimple.h"

/** 
 * Creates a expression for the given block
 */
Expression::Expression( int oper,
                        my_simple_reg *src1,
                        my_simple_reg *src2) {

  _oper = oper;
  assert( src1);
  //_src1 = src1;
  //_src2 = src2;
  _src1 = copyMySimpleReg(src1);
 if( src2) {
   _src2 = copyMySimpleReg(src2);
 }else{
   _src2 = 0;
 }
}

/**
 * Destroys the expression
 */
Expression::~Expression() { 
  if(_src1) {
    deleteMySimpleReg(_src1);
    _src1 = 0;
  }
  if(_src2) {
    deleteMySimpleReg(_src2);
    _src2 = 0;
  }
}

/**
 * Copy C'tor
 */
Expression::Expression(const Expression& rhs) {
 
  _oper = rhs._oper;
  if( rhs._src1 ) {
    _src1 = copyMySimpleReg(rhs._src1);
  }
  if( rhs._src2 ) {
    _src2 = copyMySimpleReg(rhs._src2);
  }
}
 
/**
 * Assignment operator
 */
Expression Expression::operator= (const Expression &rhs) {
 
  if( this == &rhs ) {
    return *this;
  }
 
  if( _src1 ) {
    deleteMySimpleReg(_src1);
    _src1 = 0;
  }
  if( _src2 ) {
    deleteMySimpleReg(_src2);
    _src2 = 0;
  }
  _oper = rhs._oper;
  if( rhs._src1 ) {
    _src1 = copyMySimpleReg(rhs._src1);
  }
  if( rhs._src2 ) {
    _src2 = copyMySimpleReg(rhs._src2);
  }
  return *this;
}

 
/**
 * Overload the == operator
 */
bool Expression::operator== (const Expression &rhs) const {
  bool status = false;

  if( _oper == rhs._oper) {
     if( (_src1 != 0) && ( rhs._src1 != 0 ) ) {
       status = isEqual_reg(_src1, rhs._src1);
     }
     if(status) {
       if( (_src2 == 0) && ( rhs._src2 == 0 ) ) {
         status = true;
       }else{
         status = isEqual_reg( _src2, rhs._src2);
       }
     }

  }
  
  return status;
}

