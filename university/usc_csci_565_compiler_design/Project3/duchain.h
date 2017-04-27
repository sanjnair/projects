#if !defined __duchain_H__
#define __duchain_H__

#include "mysimple.h"
#include "prjsupport.h"

/**
 * Encapsulates the duchain of the node
 */
class DuChain  {
public:

  /** 
   * Constructs a DuChain  with id and block
   */
  DuChain( my_simple_reg *reg,
           int def );
   
  /**
   * Copy C'tor
   */
  DuChain (const DuChain & rhs);
 
  /**
   * Assignment operator
   */
  DuChain  operator= (const DuChain  &rhs);

  /**
   * Destroys the duchain
   */
  ~DuChain ();

 /**
  * equality operator
  */
  //inline bool operator==(const DuChain& y) {
  //  return (this->_def == y._def);
  //}
 
  /**
   * less than operator
   */
  inline bool operator<(const DuChain& y) {
    return this->_def < y._def;
  }

  /**
   * returns the definition
   */
  int getDef() const;

  /**
   * Returns the use list
   */
  IntList getUseList() const;

  /**
   * Gets the register
   */
  my_simple_reg *getReg() const;

  /**
   * Adds a use
   */
  void addUse( int use );

private:
    int _def;
    IntList _useList;
    my_simple_reg *_reg;
};

typedef std::vector<DuChain>DuChainVector;

#endif //__duchain_H__
