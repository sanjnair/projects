#include "duchain.h"
#include "vertex.h"

/**
 * Constructs a DuChain  with id and block
 */
DuChain::DuChain ( my_simple_reg *reg,
                   int def ) {
  assert(reg);

  _reg = copyMySimpleReg( reg );
  _def = def;
}

/**
 * Destroys the duchain
 */
DuChain::~DuChain () {
  if( _reg ) {
    deleteMySimpleReg(_reg);
  }
  _def = -1;
}
 
/**
 * Copy C'tor
 */
DuChain::DuChain (const DuChain & rhs) {
  _def = rhs._def;
  _useList = rhs._useList;

  if( rhs._reg) {
    _reg = copyMySimpleReg( rhs._reg );
  }

}
 
/**
 * Assignment operator
 */
DuChain DuChain::operator= (const DuChain  &rhs) {
  if( this == &rhs ) {
    return *this;
  }

  _def = rhs._def;
  _useList = rhs._useList;

  if( _reg ) {
    deleteMySimpleReg(_reg);
  }
  if( rhs._reg) {
    _reg = copyMySimpleReg( rhs._reg );
  }

  return *this;
}
 
/**
 * returns the definition
 */
int DuChain::getDef() const {
  return _def;
}
 
/**
 * Returns the use list
 */
IntList DuChain::getUseList() const {
  return _useList;
}

/**
 * Gets the register
 */
my_simple_reg* DuChain::getReg() const {
  return _reg;
}
 
/**
 * Adds a use
 */
void DuChain::addUse( int use ) {
  if(! Vertex::isItemInList( _useList, use )) {
    _useList.push_back(use);
  }
}
