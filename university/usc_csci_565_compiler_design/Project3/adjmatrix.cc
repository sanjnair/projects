#include "adjmatrix.h"
#include "regspec.h"
#include "prjutil.h"

/**
 * Constructs a AdjMatrix  with id and block
 */
AdjMatrix::AdjMatrix( WebList &webList, 
                      BitVectorMap &inMap,
                      BitVectorMap &outMap ) {

  _webList = webList;
  _inMap = inMap;
  _outMap = outMap;

  int i = 0; 
  int nRegs = RegSpec::getInstance()->getNumMachineRegs();
  _dim = _webList.size() + nRegs;

  //map of machine regnum Vs Index in the array
  for(i=0; i<nRegs;i++ ) {
    _mRegIndexMap[i+1] = i;
  }

  i = 0; 
  //map of symbolic regnum Vs Index in the array
  for( WebList::const_iterator it = _webList.begin();
       it != _webList.end();
       it++ ) {
   int regNum = it->getWebReg()->num;
   _sRegIndexMap[regNum] = i + nRegs; 
   i++;
  }

  //initialize the matrix
  for(i=0; i<_dim;i++){
    BoolVector vct;
    for(int j=0;j<_dim;j++){
      vct.push_back(false);  
    }
    _adjMatrix.push_back(vct);
  }
}

/**
 * Destroys the adjmatrix
 */
AdjMatrix::~AdjMatrix () {
}

/**
 * builds the adjacency matrix
 */
void AdjMatrix::build() {
  //first make all registers interfere with each other
  int numMachineRegs = RegSpec::getInstance()->getNumMachineRegs();
  int i=0;
  int j=0;
  for(i=0; i<numMachineRegs;i++){
    for(j=0;j<numMachineRegs;j++) {
      if( i != j ) {
        _adjMatrix[i][j]= true;
      }
    }
  }

  //look for the webs which is defined and not used anywhere.
  //if that is the case, then turn on that bit in both in and
  //out map of the LVA
  // s3 defined at 1 used at none
  // update statement 1 of the in & out map at position 3 to true

  i = 0;
  IntList defList;
  for( WebList::const_iterator it = _webList.begin();
       it != _webList.end();
       it++ ) {
    if(it->getUseList().size() == 0 ) {
      defList = it->getDefList();
      for(IntList::const_iterator listIt = defList.begin();
          listIt != defList.end();
          listIt++ ) {
        if( 0 != *listIt ) { //shouldn't happen, but if it happens, map index will BOMB!
          int bitPos = it->getWebReg()->num - 1;
          int index = *listIt;
          BitVector &outBt = _outMap[index]; 
          outBt.setItem(bitPos, true );
        }
      }
    } 
  }

  buildInterference(_inMap );
  buildInterference(_outMap );

  //check for the machine register interference with 
  //symbolic registers
  buildRegisterInterference();
}

/**
 * Builds the interference for the bit vector
 */
void AdjMatrix::buildInterference( BitVector &bitVector ) {
  
  int nRegs = RegSpec::getInstance()->getNumMachineRegs();
  bool row = false;
  bool col = false;

  for(int i=0; i<bitVector.size();i++ ) {
    row = bitVector.item(i);
    if(row) {
      for(int j=0; j<bitVector.size();j++) {
        if( i != j ) {
          col = bitVector.item(j);
          if(col) {
            _adjMatrix[i+nRegs][j+nRegs] = true;
          }
        }
      }
    }
  } 
}

/**
 * Builds the interference for the live variable map 
 */
void AdjMatrix::buildInterference( BitVectorMap &bitMap ) {
  for(int i=0; i<bitMap.size();i++ ) {
    buildInterference(bitMap[i]);
  } 
}

/**
 * Builds the machine register to symbolic register
 * interference
 */
void AdjMatrix::buildRegisterInterference() {
  int nRegs = RegSpec::getInstance()->getNumMachineRegs();
  for(int i=0; i<nRegs;i++) {
    if( hasConstraints(i+1) ) { //register number starts with 1
      interferenceWithConstraints(i+1);
    }else {
      interferenceWithNoConstraints(i+1);
    }
  }
}

/**
 * Makes the constraint for the Machine Register that
 * have no constraints
 */
void AdjMatrix::interferenceWithNoConstraints( int machineReg ) {
  const ConstraintVector &vct = RegSpec::getInstance()->getConstraintVector();
 
  int mRegIndex = _mRegIndexMap[machineReg];
  int sRegIndex = -1;

  for( ConstraintVector::const_iterator it = vct.begin();
       it != vct.end();
       it ++ ) {
    sRegIndex = _sRegIndexMap[it->_symReg]; 

    _adjMatrix[mRegIndex][sRegIndex] = true;
    _adjMatrix[sRegIndex][mRegIndex] = true;
  }
}

/**
 * Makes the constraint for the Machine Register that
 * have no constraints
 */
void AdjMatrix::interferenceWithConstraints( int machineReg ) {

  int mRegIndex = _mRegIndexMap[machineReg];
  int sRegIndex = -1;
  IntList list;

  const ConstraintVector &vct = RegSpec::getInstance()->getConstraintVector();
  for( ConstraintVector::const_iterator it = vct.begin();
       it != vct.end();
       it ++ ) {
    if( it->_machineReg == machineReg ) {
      sRegIndex = _sRegIndexMap[it->_symReg];
      list.push_back(sRegIndex);
    }
  }

  BoolVector colVector(_dim, false);
  //union all the symbolic register columns
  for( IntList::const_iterator listIt = list.begin();
       listIt != list.end(); listIt++ ) {
    sRegIndex = *listIt;
    colVector = unionVector(colVector, _adjMatrix[sRegIndex]);
  } 

  //now set all the symbolic register in this vector to false
  for( IntList::const_iterator listIt = list.begin();
       listIt != list.end(); listIt++ ) {
    sRegIndex = *listIt; 
    colVector[sRegIndex] = false;
  }

  assert(colVector.size() == _adjMatrix.size());
  //now set this vector as the interference for the real register
  int nRegs = RegSpec::getInstance()->getNumMachineRegs();
  for(int i=nRegs; i<colVector.size(); i++ ) {
    _adjMatrix[i][mRegIndex] = colVector[i];
    _adjMatrix[mRegIndex][i] = colVector[i];
  }
}

/**
 * Unions the boolean vector
 */
BoolVector AdjMatrix::unionVector( BoolVector &b1, 
                                   BoolVector &b2,
                                   int startFrom ) {

  BoolVector retVector = b1;
  for( int i=startFrom; i<retVector.size(); i++ ) {
    retVector[i] = b1[i] || b2[i];
  }
  return retVector;
}

/**
 * returns true if the machine register has constraint
 * in the reg file
 */
bool AdjMatrix::hasConstraints(int machineReg ) {
  const ConstraintVector &vct = RegSpec::getInstance()->getConstraintVector();

  for( ConstraintVector::const_iterator it = vct.begin();
       it != vct.end();
       it ++ ) {

    if( it->_machineReg == machineReg ) {
      return true;
    }
  }
  return false;
}

/**
 * Returns true if two symbolic registers interfere with
 * each other
 */
bool AdjMatrix::isSymRegInterfere( int reg1, int reg2 ) {

  if( (_sRegIndexMap.find(reg1) != _sRegIndexMap.end()) &&
      (_sRegIndexMap.find(reg2) != _sRegIndexMap.end() ) ) {
    int reg1Index = _sRegIndexMap[reg1];
    int reg2Index = _sRegIndexMap[reg2];

    return (_adjMatrix[reg1Index][reg2Index]);
  }
  return false;
}

/**
 * Removes an item from the BoolVector identified by 
 * the index
 */
void AdjMatrix::removeItemFromVector( BoolVector &vct, int pos ) {

  int i =0;
  for(BoolVector::iterator bIt = vct .begin();
      bIt != vct.end();
      bIt++ ) {
    if( i == pos ) {
      vct.erase(bIt);  
      break;
    }
    i++;
  }
}

/**
 * returns a new adjmatrix for the coalesced register.
 * It is the caller's reposibility to delete the 
 * newly created object
 */
AdjMatrix* AdjMatrix::getCoalescedMatrix( my_simple_reg *coalescedReg,
                                          my_simple_reg *coalescedWithReg ) {

  assert(coalescedReg);
  assert(coalescedWithReg);


  //first get the weblist and remove the coalesced register from there
  WebList webList = _webList;
  bool removed = true;

  for(WebList::iterator it = webList.begin(); 
      it != webList.end(); it++ ) {
    my_simple_reg *webReg = it->getWebReg();
    assert(webReg);
    if( (webReg->num == coalescedReg->num) &&
        (webReg->kind == coalescedReg->kind) ) {
      webList.erase(it);
      removed = true;
      break;
    }
  }  
  if( !removed ) {
    printf( "Unabled to find register %d in web list\n", coalescedReg->num );
    return 0;
  }

  AdjMatrix *newMatrix = new AdjMatrix( webList, _inMap, _outMap );
  int coalescedRegIndex = _sRegIndexMap[coalescedReg->num];
  int coalescedWithRegIndex = _sRegIndexMap[coalescedWithReg->num]; 

  int nRegs = RegSpec::getInstance()->getNumMachineRegs();
  int row = 0;

  /*for(int i=0; i< _adjMatrix.size(); i++ ) {
    if( i != coalescedRegIndex ) {
       BoolVector vctBool = _adjMatrix[i];
       bool coalesced = false;

       if(i == coalescedWithRegIndex) {
         vctBool = unionVector( vctBool, 
                                _adjMatrix[coalescedRegIndex],
                                nRegs -1 ); 
         coalesced = true;
         //set own entry to false
         vctBool[coalescedWithRegIndex] = false;
       }
       removeItemFromVector(vctBool, coalescedRegIndex );
       newMatrix->setRow(row, vctBool, coalesced); 
        
       row++;
    }
  }*/

  BoolVector vctBool;
  int coalRow = 0;

  for(int i=0; i< _adjMatrix.size(); i++ ) {
    if(i != coalescedRegIndex) {
      if( i == coalescedWithRegIndex) {
        coalRow = row;
      }
      vctBool.clear();
      vctBool = _adjMatrix[i];
      removeItemFromVector(vctBool, coalescedRegIndex );
      newMatrix->setRow(row, vctBool, false);

      row++;
    }
  }
  //put the coalesced row last
  vctBool = unionVector( _adjMatrix[coalescedWithRegIndex],
                         _adjMatrix[coalescedRegIndex],
                         nRegs -1 );
  //set own entry to false
  vctBool[coalescedWithRegIndex] = false;
  removeItemFromVector(vctBool, coalescedRegIndex );
  newMatrix->setRow(coalRow, vctBool, true);

  return newMatrix;
}

/**
 * Sets the row of the adjacency matrix. The size of the
 * vector must be same as the matrix dimension
 */
bool AdjMatrix::setRow( int row, BoolVector& vctRow, bool coalesced ) {

  if( vctRow.size() != _dim ) {
    printf(" size of the row is not matching with the matrix dimension: %d\n", _dim );
    return false;
  }
  _adjMatrix[row] = vctRow;

  if(coalesced ) {
    //now modify the column too
    for(int j=0; j<_dim;j++ ) {
      _adjMatrix[j][row] = vctRow[j];
    }
  }
}

/**
 * prints the lower triangular matrix
 */
void AdjMatrix::print( FILE *fd, bool lower ) const {

  int nMachineRegs = RegSpec::getInstance()->getNumMachineRegs();
  int i = 0;
  int j=0;
  WebList::const_iterator it =0;

  //first print row
  //row is for all machine regs and all but the last symbolic regs
  for( i= 0; i<nMachineRegs; i++ ) {
    fprintf(fd, "\tr%d",i+1);
  } 
  //print the symbolic regs
  for(it=_webList.begin(); it != _webList.end(); it++ ) {
    if(lower) {
      if( j != (_webList.size()-1) ) {
        fprintf(fd, "\t");
        printRegNumber(it->getWebReg(), fd);      
      }
    }else {
      fprintf(fd, "\t");
      printRegNumber(it->getWebReg(), fd);      
    }
    j++;
  } 

  fprintf(fd, "\n");
  it = _webList.begin();

  int start = (lower) ? 1 : 0;
  //now print the contents
  for(i=start;i<_dim;i++){
    if( i < nMachineRegs ) {
      fprintf(fd, "r%d",i+1);
    }else{
      printRegNumber(it->getWebReg(), fd);
      it++;
    }
    
    int end = (lower) ? i : _dim; //if lower triangular matrix is true, then
                                  // loop through i,  else print normal matrix
    for(j=0;j<end;j++){
      fprintf(fd, "\t");
      if( _adjMatrix[i][j] ) {
        fprintf(fd, "T");
      }else {
        fprintf(fd, "-");
      }
    }
    fprintf(fd, "\n");
  }
  fprintf(fd, "\n");
}

