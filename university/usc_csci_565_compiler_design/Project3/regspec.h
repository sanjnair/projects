#if !defined __regspec_H__
#define __regspec_H__

#include "mysimple.h"
#include "prjsupport.h"

class Constraint {
  public:
     Constraint();
     Constraint( int machineReg, int symReg );
     int _machineReg;
     int _symReg;
};

typedef enum FileReadStateEnum {
  READ_MACHINE_REG,
  READ_PARAMS,
  READ_BLOCK_ITER,
  READ_CONSTRAINT_NUM,
  READ_CONSTRAINTS,
  EOF_REACHED
}FileReadState;

typedef std::vector<Constraint> ConstraintVector;

/**
 * Singleton for the register specification
 */
class RegSpec  {
public:

  /**
   * Destructs the RegSpec object
   */
  ~RegSpec();

  /**
   * Gets the instance of the RegSpec
   */
  static RegSpec* getInstance();

  /**
   * Initializes the regspec. If initialization is not succesful,
   * then this method returns false
   */
  bool initialize( const char* programFile );

  /**
   * Evaluates the parameters
   */ 
  bool evalParam( int numBlocks );

  /**
   * returns the number of machine regs
   */
  inline unsigned int getNumMachineRegs() const {
    return _numMachineRegs;
  }

  /**
   * returns the number of params
   */
  inline unsigned int getNumParams() const {
    return _numParams;
  }

  /**
   * returns the number of constraints
   */
  inline unsigned int getNumConstraints() const {
    return _numConstraints;
  }

  /**
   * returns a reference to the contraint vector
   */
  inline ConstraintVector getConstraintVector() const {
    return _constraintVector;
  } 

  /**
   * Returns the loop count for the given block
   */
  inline int getLoopCount( int blockNum ) {
    return _blockLoopMap[blockNum];
  } 

private:
  /**
   * constructor is prvate for the singleton
   */
  RegSpec();

  static RegSpec *_instance;

  std::string _fileName;
  unsigned int _numMachineRegs;
  unsigned int _numParams;
  IntMap _blockLoopMap;
  int _numConstraints;
  ConstraintVector _constraintVector;
};

#endif //__regspec_H__
