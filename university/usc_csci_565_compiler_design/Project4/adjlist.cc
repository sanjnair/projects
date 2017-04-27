#include "adjlist.h"
#include "prjutil.h"
#include "regspec.h"

AdjList::AdjList( ) {
  _nints  = 0;
  _color = INT_MIN;
  _disp = INT_MIN;
  _spcost = FLT_MAX;
  _spill = false;
};

/**
 * Destroys the adjlist
 */
AdjList::~AdjList () {

}

/**
 * returns true, if the regs are equal
 */
bool AdjList::isRegEqual( const my_simple_reg &reg1,
                          const my_simple_reg &reg2 ) {

  return ( ( (reg1.num == reg2.num) && 
             (reg1.kind == reg2.kind) ) ? true : false );
}

/**
 * returns true, if the item is in the reglist
 */
bool AdjList::isItemInRegList( const MyRegList &list,
                               const my_simple_reg &reg ) {
  for( MyRegList::const_iterator it= list.begin();
       it != list.end(); it++ ) {
    if( isRegEqual( *it, reg) ) {
      return true;
    }
  }
  return false;
}


/**
 * Returns the position of the item in the adjvector
 */
int AdjList::getPos( const AdjListVector &adjVector, 
                     const my_simple_reg &reg ) {
  int pos = -1;
  int i=0;

  for( AdjListVector::const_iterator it = adjVector.begin();
       it != adjVector.end(); it++ ) {
    if( isRegEqual(it->_reg, reg) ) {
      pos = i;
      break;
    }
    i++;
  }
  return pos;
} 
 
/**
 * removes the item form the list 
 */
void AdjList::removeItemFromList( MyRegList &list,
                                  const my_simple_reg &reg ) {

  for( MyRegList::iterator it = list.begin();
       it != list.end(); it++ ) {
    if( isRegEqual(*it, reg) ) {
      list.erase(it);
      break;
    }
  }
}

/**
 * Builds the adjacency list, 
 */
void AdjList::build( const AdjMatrix &adjMatrix,
                     IntMap &regMap,
                     AdjListVector &adjVector ) {

  int nregs = RegSpec::getInstance()->getNumMachineRegs();
  int i =0;
  int j = 0;
  my_simple_reg reg;

  adjVector.clear();
  for(i=0; i<nregs;i++ ) {
    AdjList adjList;
    reg.num = i + 1;
    reg.kind = PSEUDO_REG;; 
    adjList._reg = reg;
    adjList._nints = 0;
    adjList._color = INT_MIN;
    adjList._disp = INT_MIN;
    adjList._spcost = FLT_MAX;
    adjVector.push_back(adjList); 
  }
  const WebList &webList = adjMatrix.getWebList();
  for(WebList::const_iterator it = webList.begin();
      it != webList.end(); it++ ) {
    AdjList adjList;
    reg.num = it->getWebReg()->num;
    reg.kind = SYMBOLIC_REG; 
    adjList._reg = reg;
    adjList._nints = 0;
    adjList._color = INT_MIN;
    adjList._disp = INT_MIN;
    if( -1 != regMap[reg.num]) {
      adjList._spcost = (float)regMap[reg.num];
    }
    adjVector.push_back(adjList); 
  }

  int size = nregs + webList.size(); 
  for(i=1; i< size; i++ ) {
    for(j=0; j<size-1; j++ ) {
      if( adjMatrix.getValue(i,j) ) {
        if(! isItemInRegList( adjVector[i]._adjnds, adjVector[j]._reg) ) {
          adjVector[i]._adjnds.push_back(adjVector[j]._reg);
          adjVector[i]._nints += 1;
        }
        if(! isItemInRegList( adjVector[j]._adjnds, adjVector[i]._reg) ) {
          adjVector[j]._adjnds.push_back(adjVector[i]._reg);
          adjVector[j]._nints += 1;
        }
      }
    } 
  } 
}

/**
 * prints the lower triangular matrix
 */
void AdjList::print( const AdjListVector &adjVector,
                     FILE *fd ) {
  printHeader("Adjacency List", 0, fd );

  fprintf(fd,"\tcolor\tdisp\tspcost\tnints\tadjnds\t\t\t\t\trmvadjnds\n");
  MyRegList list;
  std::string regPrefix;
  int i = 0;
  MyRegList::const_iterator listIt = 0;
  const int TAB_SIZE = 8;
  const int REG_STR_SIZE = 3; //assuming register with one decimal + prefix + space

  for(AdjListVector::const_iterator it = adjVector.begin();
     it != adjVector.end(); it++ ) {

     printRegNumber( &(it->_reg), fd ); 

     fprintf(fd, "\t");
     if( INT_MIN == it->_color ) {
       fprintf(fd, "ni");
     }else {
       fprintf(fd, "%d" , it->_color );
     }
     fprintf(fd, "\t");
     if( INT_MIN == it->_disp ) {
       fprintf(fd, "ni");
     }else {
       fprintf(fd, "%d" , it->_disp );
     }
     fprintf(fd, "\t");
     if( FLT_MAX == it->_spcost ) {
       fprintf(fd, "pi");
     }else {
       fprintf(fd, "%2.0f" , it->_spcost );
     }
     fprintf(fd, "\t");
     fprintf(fd, "%d" , it->_nints );

     list = it->_adjnds;
     list.sort();
     fprintf(fd, "\t");
     for( listIt = list.begin(); listIt != list.end(); listIt++ ) { 
       const my_simple_reg &reg_r = *listIt;
       printRegNumber( &reg_r, fd ); 
       fprintf(fd, " ");
     }
     int regsize = REG_STR_SIZE * list.size();
     int tabs = (regsize / TAB_SIZE);
     if( tabs < 5 ) {
       tabs = 5 - tabs;
       if(tabs > 0 ) {
         for(int i=0;i<tabs;i++){
           fprintf(fd, "\t");
         }
       }
     }

     list = it->_rmvadj;
     list.sort();
     for( listIt = list.begin(); listIt != list.end(); listIt++ ) {
       const my_simple_reg &reg_r = *listIt;
       printRegNumber( &reg_r, fd ); 
       fprintf(fd, " ");
     }
     fprintf(fd, "\n");
  }
} 

