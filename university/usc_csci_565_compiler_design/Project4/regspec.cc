#include "regspec.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream.h>

RegSpec *RegSpec::_instance = 0;
 
/**
 * Creates a constraint
 */
Constraint::Constraint() {
  _machineReg = -1;
  _symReg = -1;
}

/**
 * Creates a constraint
 */
Constraint::Constraint( int machineReg, int symReg ) {
  _machineReg = machineReg;
  _symReg = symReg;
} 

/**
 * Constructs the RegSpec object
 */
RegSpec::RegSpec() {

}

/**
 * Destructs the RegSpec object
 */
RegSpec::~RegSpec() {

}

/**
 * Gets the instance of the RegSpec
 */
RegSpec* RegSpec::getInstance() {
  if( 0 == _instance ) {
    _instance = new RegSpec();
  } 
  return _instance;
} 
 
/**
 * Initializes the regspec. If initialization is not succesful,
 * then this method returns false 
 */
bool RegSpec::initialize( const char* programFile ) {
  bool status = false;

  if( 0 == programFile ) {
    printf("Valid program file is not passed\n");
    return false;
  }
  _fileName = programFile;
  _fileName.replace(0,4,"reg");

  int pos = _fileName.find(".",0);
  if( pos < 0 ) {
     printf("Invalid input file. \".\" character not found.\n");
     return false;
  }
  _fileName.replace(pos+1,4,"txt");
  return true;
}

/**
 * Evaluates the parameters
 */
bool RegSpec::evalParam( int numBlocks ) {
  std::string strLine;
  ifstream file(_fileName.c_str());
  if( !file ) {
    printf("Unable to open file %s\n", _fileName.c_str() );
    return false;
  }
  int iter = 0;
  int state = READ_MACHINE_REG;
  int constraintCount = 0;
  int loopCount = 0;
  std::string strMachineReg;
  std::string strSymReg;
  int machineReg = 0;
  int symReg = 0;
  int pos = -1;
  Constraint constraint;

  while(!file.eof()) { 
   getline(file,strLine); // read a line of any length. 
     switch(state) {
       case READ_MACHINE_REG:
         _numMachineRegs = atoi(strLine.c_str());
         state = READ_PARAMS;
         break;
       case READ_PARAMS:
         _numParams = atoi(strLine.c_str());
         state = READ_BLOCK_ITER;
         break;
       case READ_BLOCK_ITER:
         loopCount = atoi(strLine.c_str());
         _blockLoopMap[iter] = loopCount;
         iter++;
           
         if( iter == numBlocks ) {
           state = READ_CONSTRAINT_NUM; 
         }
         break;
       case READ_CONSTRAINT_NUM:
        _numConstraints = atoi(strLine.c_str()); 
        if(0 == _numConstraints ) {
           state = EOF_REACHED;
         }else {
           state = READ_CONSTRAINTS;
         }
         break; 
       case READ_CONSTRAINTS:
         pos = strLine.find(" ",0);
         if( pos < 0 ) {
           printf("Invalid constraint %s\n", strLine.c_str());
           return false;
         }
         strMachineReg = strLine.substr(0,pos); 
         strSymReg = strLine.substr(pos+1,strLine.length()); 
         machineReg = atoi(strMachineReg.substr(1,1).c_str()); 
         symReg = atoi(strSymReg.substr(1,1).c_str()); 
         constraint._machineReg= machineReg;
         constraint._symReg = symReg;
         _constraintVector.push_back(constraint);
         
         constraintCount++;
         if(constraintCount == _numConstraints) {
           state = EOF_REACHED;
         }
         break;
       case EOF_REACHED:
         break;
       default:
         printf("Invalid state reached.. Invalid file content\n");
         return false;
     }
  }
  file.close(); 
  return true;
}

