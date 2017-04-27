#include "web.h"

/**
 * Constructs a Web  with id and block
 */
Web::Web(my_simple_reg *reg,
         IntList defs,
         IntList uses ) {
  assert(reg);

  _reg = copyMySimpleReg( reg );
  _webReg = copyMySimpleReg(reg);
  _webReg->kind = SYMBOLIC_REG;
  _webReg->subscript = -1;

  _defList = defs;
  _useList = uses;
}

/**
 * Destroys the duchain
 */
Web::~Web () {
  if( _reg ) {
    deleteMySimpleReg(_reg);
  }
  if( _webReg ) {
    deleteMySimpleReg(_webReg);
  }
}
 
/**
 * Copy C'tor
 */
Web::Web (const Web & rhs) {
  _defList = rhs._defList;
  _useList = rhs._useList;

  if( rhs._reg) {
    _reg = copyMySimpleReg( rhs._reg );
  }
  if( rhs._webReg) {
    _webReg = copyMySimpleReg( rhs._webReg );
  }

}
 
/**
 * Assignment operator
 */
Web Web::operator= (const Web  &rhs) {
  if( this == &rhs ) {
    return *this;
  }

  _defList = rhs._defList;
  _useList = rhs._useList;

  if( _reg ) {
    deleteMySimpleReg(_reg);
  }
  if( rhs._reg) {
    _reg = copyMySimpleReg( rhs._reg );
  }

  if( _webReg ) {
    deleteMySimpleReg(_webReg);
  }
  if( rhs._webReg) {
    _webReg = copyMySimpleReg( rhs._webReg );
  }

  return *this;
}
 
/**
 * returns the definition
 */
IntList Web::getDefList() const {
  return _defList;
}

/**
 * Sets the def list
 */
void Web::setDefList( IntList &list ) {
  _defList = list;
}
 
/**
 * Returns the use list
 */
IntList Web::getUseList() const {
  return _useList;
}

/**
 * Sets the use list
 */
void Web::setUseList( IntList &list ) {
  _useList = list;
}

/**
 * Gets the register
 */
my_simple_reg* Web::getReg() const {
  return _reg;
}

/** 
 * Returns the web reg
 */
my_simple_reg* Web::getWebReg() const {
  return _webReg;
}

 
