#include "vertex.h"
#include "myprint.h"

/** 
 * Creates a vertex for the given block
 */
Vertex::Vertex(struct basic_block *block,
               bool deleteBlock) {

  assert(block);
  _block = block;
  _firstInstr = 0;
  _lastInstr = 0;
  _deleteBlock = deleteBlock;
}

/**
 * Destroys the vertex
 */
Vertex::~Vertex() {
  my_simple_instr *myInstr = _firstInstr;

  while( myInstr != 0 ) {
    deleteMySimpleInstr( myInstr );
    myInstr = myInstr->next;
  }
  _firstInstr = 0;
  _lastInstr = 0;

  if( _deleteBlock ) {
    assert(_block);
    delete _block;
    _block = 0;
  }
}

/**
 * Copy C'tor
 */
Vertex::Vertex(const Vertex& rhs) {

  if( rhs._deleteBlock ) {
    _block = new basic_block;
    *_block = *(rhs._block);
  } 

  my_simple_instr *myInstr = 0;
  //copy the instructions
  if( 0 != rhs._firstInstr ) {
    myInstr = rhs._firstInstr;
    my_simple_instr *p = 0;
    my_simple_instr *q = 0;

    while( myInstr != 0 ) {
      p = copyMySimpleInstr( myInstr ); 
      if( q ) {
        q ->next = p;
        p->prev = q;
      }
      q = p;

      if(rhs._firstInstr == myInstr) {
        _firstInstr = p;
      }else if(rhs._lastInstr == myInstr) {
        _lastInstr = p;
      }
    }
  }
  _deleteBlock = rhs._deleteBlock;
  if( _deleteBlock ) {
    _block = new basic_block;
    *_block = *(rhs._block);
  } 
} 
 
/**
 * Assignment operator
 */
Vertex Vertex::operator= (const Vertex &rhs) {

  if( this == &rhs ) {
    return *this;
  }
  if( _deleteBlock ) {
    delete _block;
    _block = 0;
  }

  my_simple_instr *myInstr = 0;
  myInstr = rhs._firstInstr;

  while( myInstr != 0 ) {
    deleteMySimpleInstr(myInstr);
  }
  _firstInstr = 0;
  _lastInstr = 0;

  //copy the instructions
  if( 0 != rhs._firstInstr ) {
    myInstr = rhs._firstInstr;
    my_simple_instr *p = 0;
    my_simple_instr *q = 0;
 
    while( myInstr != 0 ) {
      p = copyMySimpleInstr( myInstr );
      if( q ) {
        q ->next = p;
        p->prev = q;
      }
      q = p;
 
      if(rhs._firstInstr == myInstr) {
        _firstInstr = p;
      }else if(rhs._lastInstr == myInstr) {
        _lastInstr = p;
      }
      myInstr = myInstr->next;
    }
  }
  _deleteBlock = rhs._deleteBlock;

  if( _deleteBlock ) {
    if( _block ) {
      delete _block;
    }
    _block = new basic_block;
    *_block = *(rhs._block);
  }
} 

/** 
 * returns true, if item exist in the vector
 * 
 */ 
bool Vertex::isItemInList( const IntList& list, int item ) {
  bool status = false; 
 
  for( IntList::const_iterator it = list.begin(); 
    it != list.end(); 
    it++ ) { 
 
    if( *it == item ) { 
      status = true; 
      break; 
    } 
  } 
  return status; 
}

/**
 * returns true, if the given insruction is an expression
 */
bool Vertex::isExpression(const my_simple_instr *instr ) {
  assert(instr);
  bool status = false;
 
  if( instr->dst != 0 ) {
    if(instr->src1 != 0 ) {
      //if operator is not there, then it can be a unary operator
      if( -1 == instr->oper ) {
        if( (instr->src1->cvt == 1) ||
            (instr->src1->neg == 1) ||
            (instr->src1->not == 1) ){
          status = true;
        }
      } else {
         if( 0 != instr->src2 ) {
           status = true;
         }
      }
    }
  }
  return status; 
}

/**
 * returns true, if the regiter is used as an assignment
 * from the given node.
 */
bool Vertex::isGenerated( my_simple_instr *from, my_simple_reg *src ) {
  bool status = false;
  assert(from);
  assert(src);
 
  my_simple_instr *instr = from;
  while( instr != 0 ) { 
    my_simple_reg *dst = instr->dst;

    if( 0 != dst ) {
      if(dst->num == src->num) {
        status = true;
        break;
      }
    }
    /*
    if( 0 != dst && (0 != src->reg)  ) {
      if( dst->reg->num == src->reg->num ) {
        status = true;
        break;
      }
    }*/
    instr = instr->next;
  }
 
  return status;
}
 
/**
 * Returns all the expression in a block
 */
void Vertex::getAllExpressions( ExpressionVector& list ) {
  my_simple_instr *myInstr = _firstInstr;
  while( myInstr != 0 ) {
    if(isExpression(myInstr)) {
      Expression exp( myInstr->oper, myInstr->src1, myInstr->src2);
      if( ! isItemInList(list, exp) ) {
        list.push_back(exp);
      }
    }  
    myInstr = myInstr->next;
  }

  for( ExpressionVector::const_iterator it = list.begin();
       it != list.end(); it++ ) {
  }
} 

/**
 * Inserts the new instruction before the specified instruction.
 */
void Vertex::insertInstrBefore( my_simple_instr *instr,
                                my_simple_instr *newInstr ) {
  assert(instr);
  assert(newInstr);

  newInstr->next = instr;
  newInstr->prev = instr->prev;

  //move the label if any
  newInstr->lab = instr->lab;
  instr->lab = 0;

  if( 0 != instr->prev ) {
    instr->prev->next = newInstr;
  }

  instr->prev = newInstr;

  if( _firstInstr == instr ) {
    _firstInstr = newInstr;
  }
}

/**
 * Inserts the new instruction after the specified instruction.
 */
void Vertex::insertInstrAfter( my_simple_instr *instr, 
                               my_simple_instr *newInstr ) {

  assert(instr);
  assert(newInstr);

  newInstr->prev = instr;
  newInstr->next = instr->next;

  if( 0 != instr->next ) {
    instr->next->prev = newInstr;
  }
  instr->next = newInstr;

  if( _lastInstr == instr ) {
    _lastInstr = newInstr;
  }
}


/**
 * deletes the instruction and re-arranges the link list
 */
void Vertex::deleteInstr( my_simple_instr *myInstr ) {
  assert(myInstr);

  //re-arrange the link list
  if( 0 != myInstr->prev){
    myInstr->prev->next = myInstr->next;
  }
  if( 0 != myInstr->next ) {
    myInstr->next->prev = myInstr->prev;   
  }
  if( _firstInstr == myInstr ) {
    _firstInstr = myInstr->next;
    if( (0 != myInstr->next) &&
        (0 != _lastInstr ) &&
        (myInstr->next == _lastInstr) ) {
      _lastInstr = 0;
    }
  }else if( _lastInstr == myInstr ) {
    if( (0 != myInstr->prev) &&
        (_firstInstr != myInstr->prev )) {
      _lastInstr = myInstr->prev;
    } else {
      _lastInstr = 0;
    }
  }
  deleteMySimpleInstr( myInstr );
}

/**
 * Generates the spill code, and modifies the code.
 * Ignores statement such as s3 = s3 + s2
 */
bool Vertex::genSpillCode( my_simple_instr *curInstr,
                           my_simple_reg *curReg,
                           const my_simple_reg *spillReg,
                           const my_simple_reg *realReg, 
                           bool insertAfter ) {
  assert(curInstr);
  bool generated = false;

  if( SAV_OP == curInstr->opcode ) {
    return false;
  }

  if( 0 != curReg ) {
    if( (curReg->num == spillReg->num) &&
        (curReg->kind == spillReg->kind) ) {

      my_simple_instr *newInstr = createMySimpleInstr();
      newInstr->block_no = curInstr->block_no;
      newInstr->opcode = SAV_OP;
      my_simple_reg *newSpillReg = createMySimpleReg();
      newSpillReg->num = spillReg->num;
      newSpillReg->kind = spillReg->kind;

      my_simple_reg *newRealReg = copyMySimpleReg(realReg);

      if( insertAfter ) {
        newInstr->dst = newRealReg;
        newInstr->src1 = newSpillReg;
        insertInstrAfter( curInstr, newInstr );

        generated = true;
      }else {
        if( ( 0 == curInstr->dst ) ||
            ( REAL_REG != curInstr->dst->kind) ) {
          newInstr->dst = newSpillReg;
          newInstr->src1 = newRealReg;
          insertInstrBefore( curInstr, newInstr );

          generated = true;
        }
      }
    }
  }
  return generated;
}

/**
 * Generates the spill code, and modifies the code. 
 * Ignores statement such as s3 = s3 + s2
 */
void Vertex::genSpillCode( const my_simple_reg *spillReg,
                           const my_simple_reg *realReg ) {

  my_simple_instr *myInstr = _firstInstr;
  
  while( 0 != myInstr ) {
    bool gen = false;
    //spill the dest reg
    genSpillCode( myInstr, myInstr->dst, spillReg, realReg, true ); 

    //now spill the source regs. If a source reg is inserted once,
    //don't do this step for other source registers.
    gen = genSpillCode( myInstr, myInstr->src1, spillReg, realReg, false ); 

    if( !gen ) {
      gen = genSpillCode( myInstr, myInstr->src2, spillReg, realReg, false ); 
    }
    if( !gen ) {
      gen = genSpillCode( myInstr, myInstr->proc, spillReg, realReg, false ); 
    }
    if( !gen ) {
      for(int i=0; i< myInstr->nargs; i++ ) {
        gen = genSpillCode( myInstr, myInstr->args[i], spillReg, realReg, false ); 
        if (gen) { 
          break; 
        }
      }
    }
    myInstr = myInstr->next;
  }
}

/**
 * Returns generated expressions by the block
 */
void Vertex::getGenExpressions( const ExpressionVector& list,
                                BitVector &genVector ) {
  my_simple_instr *myInstr = _firstInstr;
  while( myInstr != 0 ) {
    bool gen = false;
    if(isExpression(myInstr)) {
      gen = isGenerated( myInstr, myInstr->src1); 
      if( (! gen) && (0!=myInstr->src2) ) {
        gen = isGenerated( myInstr, myInstr->src2);
      } 
      if( ! gen ) {
        Expression exp( myInstr->oper, myInstr->src1, myInstr->src2);
        int pos = getPos( list, exp );
        assert( pos != -1 );
        genVector.setItem(pos,true);
      }
    }
    myInstr = myInstr->next;
  }
} 

/**
 * Returns the killed expression by the block
 */
void Vertex::getKilledExpressions(
                           const ExpressionVector &list,
                           const BitVector  &genVector,
                           BitVector &killVector ) {
 
  //loop through the instructions to find an assignment. Once found,
  //search the list for the usage. If used, modify the bit vector
  my_simple_instr *myInstr = _firstInstr;
  my_simple_reg *dst = 0;
  ExpressionVector::const_iterator it = 0;

  while( myInstr != 0 ) {
    int count = 0;
    dst = myInstr->dst;
    if( 0 != dst ) {
      for( it=list.begin(); it!=list.end(); it++ ) {
        assert( (*it)._src1);
        if(dst->num == (*it)._src1->num) {
          killVector.setItem(count, true);
        } else if( (0 != (*it)._src2) &&
                   (dst->num == (*it)._src2->num) ) {
          killVector.setItem(count, true);
        }
        count++;
      }
    }
    myInstr = myInstr->next;
  }
  killVector = killVector - genVector;

  /* 
  while( myInstr != 0 ) {
    int count = 0;
    dst = myInstr->dst;
    if( 0 != dst ) {
      for( it=list.begin(); it!=list.end(); it++ ) {
        assert( (*it)._src1);
        if( (0 != (*it)._src1->reg) &&
            (dst->reg->num == (*it)._src1->reg->num) ) {
          killVector.setItem(count, true);
        } else if( (0 != (*it)._src2) &&
                   (0 != (*it)._src2->reg ) &&
                   (dst->reg->num == (*it)._src2->reg->num) ) {
          killVector.setItem(count, true);
        }
        count++;
      }   
    }
    myInstr = myInstr->next;
  }
  killVector = killVector - genVector;
  */
}

/**
 * Gets the definitions for this block
 */
void Vertex::getDefinitions( IntList &list ) {
  my_simple_instr *myInstr = _firstInstr;
  my_simple_reg *dst = 0;

  list.clear();
  while(myInstr != 0) {
    dst = myInstr->dst;
    if( dst != 0 ) {
      if( ! isItemInList(list, dst->num )) {
        list.push_back( dst->num);
      }
    }    
    myInstr = myInstr->next;
  }
}

/**
 * Adds a variable to map. This is a helper method
 */
void Vertex::addVariableToMap(  
                   IntMap &allVars,
                   my_simple_reg *reg ) {
  if( (0 != reg) && (reg->num >0) ) {
    allVars[reg->num] = reg->kind;
  }
}

/**
 * Returns all variables and its kind in the block 
 */
void Vertex::getAllVariables( IntMap &allVars ) {

  my_simple_instr *myInstr = _firstInstr;

  while(myInstr != 0) {
    addVariableToMap( allVars, myInstr->dst);
    addVariableToMap( allVars, myInstr->src1);
    addVariableToMap( allVars, myInstr->src2);
    addVariableToMap( allVars, myInstr->proc);

    for( int i=0; i< myInstr->nargs; i++ ){
      addVariableToMap( allVars, myInstr->args[i]);
    }
    myInstr = myInstr->next;
  }
}

/**
 * Returns the position of the expression in the list 
 */
int Vertex::getPos( const ExpressionVector &list,
            const Expression &exp ) {

  ExpressionVector::const_iterator it = 0;
  int count = 0;

  for( it = list.begin(); it != list.end(); it++ ) {
    if( exp == *it ) {
      return count;
    }  
   count++;
  } 
  return -1;
}

/**
 * Returns the instruction where this temporary register is used
 */
my_simple_instr* Vertex::findInstr( my_simple_instr *instr, 
                                    my_simple_reg *dst ) {

  assert (instr);
  assert (dst);
  assert (dst->reg);

  //start looking from the next instruction
  my_simple_instr *curInstr = instr->next;
  
  while( curInstr != 0 ) {
    if( (0 != curInstr->src1) && 
        (curInstr->src1->num == dst->num) ) {
        return curInstr;
    }
    if( (0 != curInstr->src2)&&
        (curInstr->src2->num == dst->num) ) {
      return curInstr;
    }
    if( (0 != curInstr->proc) &&
        (curInstr->proc->num == dst->num) ) {
      return curInstr;
    }
    int nargs = curInstr->nargs;
    if( nargs  > 0 ) {
      my_simple_reg *my_reg = 0;
      for(int i=0; i<nargs;i++) {
        my_reg = curInstr->args[i];
        assert(my_reg);
        if( my_reg->num == dst->num) {
          return curInstr;
        }
      }
    }
    curInstr = curInstr->next;
  }

  /*
  while( curInstr != 0 ) {
    if( (0 != curInstr->src1) && 
        ( 0 != curInstr->src1->reg )  ) {
      if( curInstr->src1->reg->num == dst->reg->num ) {
        return curInstr; 
      }
    }
    if( (0 != curInstr->src2)&& 
              (curInstr->src2->reg->num == dst->reg->num) ) {
      return curInstr;  
    }
    if( (0 != curInstr->proc) &&
              (0 != curInstr->proc->reg) && 
              (curInstr->proc->reg->num == dst->reg->num) ) {
      return curInstr;
    }
    int nargs = curInstr->nargs;
    if( nargs  > 0 ) {
      my_simple_reg *my_reg = 0;
      for(int i=0; i<nargs;i++) {
        my_reg = curInstr->args[i];
        assert(my_reg); 
        if( my_reg->reg  && 
          (my_reg->reg->num == dst->reg->num )) {
          return curInstr;
        }
      }        
    }
    curInstr = curInstr->next;
  } */ 

  return 0;
}

/**
 * Combines the instructions
 * @param startLine   Last instruction line number
 */
void Vertex::combineInstr(int &lastInstrLineNo,
                          int &lastCombinedLineNo) {
  initInstr( lastInstrLineNo);

  my_simple_instr *myInstr = 0;
  my_simple_instr *nextInstr = 0;
  simple_reg *reg = 0;

  myInstr = _firstInstr;
 
  if( NULL != myInstr ) {
    while( myInstr != 0 ) {
      bool combined = false;
      assert( myInstr);

      switch(myInstr->opcode) {
      //----------------------------------------------------------------------------
        case LDC_OP:
          assert(myInstr->dst);
          reg = myInstr->dst->reg;
          assert(reg);
          if( TEMP_REG == reg->kind ) {
            //find the statement that uses this register
            nextInstr = findInstr( myInstr, myInstr->dst);
            if( 0 != nextInstr ) {
              combined = combineInstrInternal( myInstr, nextInstr);
            } 
          }
      //----------------------------------------------------------------------------
        case JMP_OP:
          break;
        case BTRUE_OP:
        case BFALSE_OP:
          break;
        case CALL_OP:
          break;
        case MCPY_OP:
          break;
        case MBR_OP:
          break;
      //----------------------------------------------------------------------------
        case LABEL_OP:
          if( 0 != myInstr->next ) {
            combined = combineInstrInternal( myInstr, myInstr->next);
          }
          break;
      //----------------------------------------------------------------------------
        case RET_OP:
          break;
        case CVT_OP:
        case NEG_OP:
        case NOT_OP:
          assert(myInstr->dst);
          reg = myInstr->dst->reg;
          assert(reg);
          if( TEMP_REG == reg->kind ) {
            //find the statement that uses this register
            nextInstr = findInstr( myInstr, myInstr->dst);
            if( 0 != nextInstr ) {
              combined = combineInstrInternal( myInstr, nextInstr);
            }
          }
          break;
      //----------------------------------------------------------------------------
        case CPY_OP:
          break;
      //----------------------------------------------------------------------------
        default:
          assert(myInstr->dst);
          reg = myInstr->dst->reg;
          assert(reg);
          if( TEMP_REG == reg->kind ) {
            //find the statement that uses this register
            nextInstr = findInstr( myInstr, myInstr->dst);
            if( 0 != nextInstr ) {
              combined = combineInstrInternal( myInstr, nextInstr);
            }
          }
      }
      //----------------------------------------------------------------------------
      nextInstr = myInstr->next;
      if( combined ) {
        //remove the old node from the list
        if( 0 != myInstr->prev ) {
          myInstr->prev->next = nextInstr;
        }
        if( 0 != nextInstr ) {
          nextInstr->prev = myInstr->prev;
        }
        if( _firstInstr == myInstr ) {
          _firstInstr = nextInstr;
        }else if( _lastInstr == myInstr ) {
          _lastInstr = myInstr->prev;
        }
        //delete the node
        // TODO memory leak here!! use deleteMySimpleInstr method
        deleteMySimpleInstr(myInstr);
        //delete myInstr;
      }
      myInstr = nextInstr;
    }
  }
  numberInstructions(lastCombinedLineNo); 
}

/**
 * number the instructions
 */
void Vertex::numberInstructions( int &lastInstrNumber ) {
  my_simple_instr *myInstr = _firstInstr;
  while( myInstr != 0 ) {
    lastInstrNumber++;
    myInstr->new_instruction_no = lastInstrNumber;
    myInstr = myInstr->next;
  }
}

/**
 * number the old instruction as the number of new instructions
 */
void Vertex::numberOldInstructions() {
  my_simple_instr *myInstr = _firstInstr;
  while( myInstr != 0 ) {
    myInstr->old_instruction_no = myInstr->new_instruction_no;
    myInstr = myInstr->next;
  }
} 


/**
 * Combines instructions from and to and the result is stored in to
 */
bool Vertex::combineInstrInternal( my_simple_instr *from, my_simple_instr *to ) {
  assert(from);
  assert(to);

  bool status = false;
  simple_op_enum opcode;

  switch(from->opcode) {
    //----------------------------------------------------------------------------
    case LDC_OP:
      switch( to->opcode ) {
        case JMP_OP:
          break; 
        case BTRUE_OP:
        case BFALSE_OP:
          if( to->src1 ) {
            if( from->dst->num == to->src1->num ) {
              deleteMySimpleReg( to->src1);
              to->src1 = copyMySimpleReg(from->src1);
              to->lab = from->lab;
              status = true;
            }
          }
          break;
        case CALL_OP:
          if(to->proc) {
          //if( (to->proc) && ( to->proc->reg) ) {
            //if( from->dst->reg->num == to->proc->reg->num ) {
            if( from->dst->num == to->proc->num ) {
              deleteMySimpleReg( to->proc);
              to->proc = copyMySimpleReg(from->src1);
              status = true;
            }
          }
          for(int i=0; i<to->nargs;i++) {
            //if( from->dst->reg->num  == to->args[i]->reg->num) {
            if( from->dst->num  == to->args[i]->num) {
              deleteMySimpleReg( to->args[i]);
              to->args[i] = copyMySimpleReg(from->src1);
              status = true;
              break; 
            }
          }
          if(status) {
            to->lab = from->lab;
          }
          break;
        case MBR_OP:
          break;
        case LABEL_OP:
          to->lab = from->lab;
          break;
        case RET_OP:
          if( to->src1 ) {
            //if( from->dst->reg->num == to->src1->reg->num ) {
            if( from->dst->num == to->src1->num ) {
              deleteMySimpleReg( to->src1);
              to->src1 = copyMySimpleReg(from->src1);
              to->lab = from->lab;
              status = true;
            }
          }
          break;
        case CVT_OP:
        case NEG_OP:
        case NOT_OP:
        case CPY_OP:
          if( to->src1 ) {
            //if( from->dst->reg->num == to->src1->reg->num ) {
            if( from->dst->num == to->src1->num ) {
              deleteMySimpleReg( to->src1);
              to->src1 = copyMySimpleReg(from->src1);
              to->lab = from->lab;
              status = true;
            }
          }
          break;
        default:
          opcode = (simple_op_enum)to->opcode; 
          //binary base instructions
          assert(simple_op_format(opcode) == BASE_FORM); 

          //if( from->dst->reg->num == to->src1->reg->num ) {
          if( from->dst->num == to->src1->num ) {
            deleteMySimpleReg( to->src1);
            to->src1 = copyMySimpleReg(from->src1);
            to->lab = from->lab;
            status = true;
          //} else if(from->dst->reg->num == to->src2->reg->num ) {
          } else if(from->dst->num == to->src2->num ) {
            to->lab = from->lab;
            deleteMySimpleReg( to->src2);
            to->src2 = copyMySimpleReg(from->src1);
            status = true;
          }
      }
    //----------------------------------------------------------------------------
    case JMP_OP:
      break;
    case BTRUE_OP: 
    case BFALSE_OP:
      break;
    //----------------------------------------------------------------------------
    case CALL_OP:
      break;
    //----------------------------------------------------------------------------
    case MBR_OP: 
      break;
    //----------------------------------------------------------------------------
    case LABEL_OP:
      to->lab = from->lab;
      status = true;
      break; 
    case RET_OP:
      break;
    //----------------------------------------------------------------------------
    case CPY_OP:
    case CVT_OP:
    case NEG_OP:
    case NOT_OP:
      switch( to->opcode ) {
        case CALL_OP:
        case JMP_OP:
        case LABEL_OP:
        case LDC_OP:
        case MCPY_OP:
        case CVT_OP:
        case NEG_OP:
        case NOT_OP:
          break;
        case BTRUE_OP: 
        case BFALSE_OP:
        case RET_OP:
        case CPY_OP:
          //if( from->dst->reg->num == to->src1->reg->num ) {
          if( from->dst->num == to->src1->num ) {
            if( to->src1 ) {
              deleteMySimpleReg( to->src1);
            }
            to->src1 = copyMySimpleReg(from->src1);
            if(from->lab) {
              to->lab = from->lab;
            }
            status = true;
          }
          break;
        default: 
          //if( from->dst->reg->num == to->src1->reg->num ) {
          if( from->dst->num == to->src1->num ) {
            if( to->src1 ) {
              deleteMySimpleReg( to->src1);
            }
            to->src1 = copyMySimpleReg(from->src1);
          //} else if( from->dst->reg->num == to->src2->reg->num ) {
          } else if( from->dst->num == to->src2->num ) {
            if( to->src2 ) {
              deleteMySimpleReg( to->src2);
            }
            to->src2 = copyMySimpleReg(from->src1);
          }
          if(from->lab) {
            to->lab = from->lab;
          }
          status = true;
      }
    //----------------------------------------------------------------------------
    default:
      //binary operators
      opcode = (simple_op_enum)from->opcode;
      assert(simple_op_format(opcode) == BASE_FORM);
      switch( to->opcode ) {
        case LDC_OP:
        case JMP_OP:
        case CALL_OP:
        case MBR_OP:
        case LABEL_OP:
        case RET_OP:
          break;
        case CVT_OP:
        case NEG_OP:
        case NOT_OP:
          //if( from->dst->reg->num == to->src1->reg->num ) {
          if( from->dst->num == to->src1->num ) {
            if( to->src1 ) {
              deleteMySimpleReg( to->src1);
            }
            to->src1 = copyMySimpleReg(from->src1);
          //} else if( from->dst->reg->num == to->src2->reg->num ) {
          } else if( from->dst->num == to->src2->num ) {
            if( to->src2 ) {
              deleteMySimpleReg( to->src2);
            }
            to->src2 = copyMySimpleReg(from->src2);
          }
          to->oper = from->opcode;
          to->lab = from->lab;
          status = true;
          break;
        case CPY_OP:
        case BTRUE_OP:
        case BFALSE_OP:
          //if( from->dst->reg->num == to->src1->reg->num ) {
          if( from->dst->num == to->src1->num ) {
            if( to->src1 ) {
              deleteMySimpleReg( to->src1);
            }
            to->src1 = copyMySimpleReg(from->src1);
            if( to->src2 ) {
              deleteMySimpleReg( to->src2);
            }
            to->src2 = copyMySimpleReg(from->src2);
            to->oper = from->opcode;
            to->lab = from->lab;
            status = true;
          }
          break;
        default:
          break;
      }
  }
  return status;
}


/**
 * Initializes the instruction to my_simple_instr format
 */
void Vertex::initInstr(int &lastInstrLineNo ) {
  simple_instr *l = 0;
  simple_instr *instr = 0;
  my_simple_instr *my_instr = 0;
  my_simple_instr *cur_instr = 0;
  my_simple_reg *my_reg = 0;
  simple_reg *reg = 0;
  int nargs = 0;
 
  l = _block->last;
  instr = _block->first;
 
  if( NULL != instr ) {
    while( instr != l->next ) {
      lastInstrLineNo++;
      my_instr = createMySimpleInstr();

      my_instr->opcode = instr->opcode;
      my_instr->old_instruction_no = lastInstrLineNo;
      my_instr->new_instruction_no = lastInstrLineNo;
      my_instr->block_no = _block->num;

      switch( instr->opcode ) {
        case LDC_OP:
          //dst register
          reg = instr->u.ldc.dst;
          my_reg = createMySimpleReg(reg);
          my_instr->dst = my_reg;
         
          //immed_value register
          my_reg = createMySimpleReg();
          my_reg->reg = 0;
          my_reg->is_immed = 1;
          my_reg->value = instr->u.ldc.value;
          my_instr->src1 = my_reg;
          break;
        case JMP_OP:
          my_instr->target = instr->u.bj.target;
          break;
        case BTRUE_OP:
        case BFALSE_OP:
          reg = instr->u.bj.src;
          my_reg = createMySimpleReg(reg);
          my_instr->src1 = my_reg;

          my_instr->target = instr->u.bj.target;
          break;
        case CALL_OP:
          reg = instr->u.call.dst;
          if( reg ) {
            my_reg = createMySimpleReg(reg);
            my_instr->dst = my_reg;
          }
          //proc 
          reg = instr->u.call.proc;
          if( reg ) {
            my_reg = createMySimpleReg(reg);
            my_instr->proc = my_reg;
          }
          //params
          nargs = instr->u.call.nargs;
          my_instr->nargs = nargs;
          if( nargs > 0 ) {
            my_instr->args = new my_simple_reg*[nargs];
            for(int n = 0; n < nargs; n++) {
              reg = instr->u.call.args[n];
              my_reg = createMySimpleReg(reg);
              my_instr->args[n] = my_reg;
            }
          }
          break;
        case LABEL_OP:
          my_instr->lab = instr->u.label.lab;
          break;
        case RET_OP:
          if (instr->u.base.src1 != NO_REGISTER) {
            reg = instr->u.base.src1; 
            my_reg = createMySimpleReg(reg);
            my_instr->src1 = my_reg;
          }
          break;
        case CVT_OP:
        case CPY_OP:
        case NEG_OP:
        case NOT_OP:
          reg = instr->u.base.dst;
          my_reg = createMySimpleReg(reg);
          my_instr->dst = my_reg;
 
          reg = instr->u.base.src1;
          my_reg = createMySimpleReg(reg);
          if( CVT_OP == instr->opcode ) {
            my_reg->cvt = 1;
          }else if( NEG_OP == instr->opcode ) {
            my_reg->neg = 1;
          }else if( NOT_OP == instr->opcode ) {
            my_reg->not = 1;
          } 
          my_instr->src1 = my_reg;

          break;
        default:
          reg = instr->u.base.dst;
          my_reg = createMySimpleReg(reg);
          my_instr->dst = my_reg; 

          reg = instr->u.base.src1;
          my_reg = createMySimpleReg(reg);
          my_instr->src1 = my_reg; 

          reg = instr->u.base.src2;
          my_reg = createMySimpleReg(reg);
          my_instr->src2 = my_reg; 
          break;
      }
      if( 0 == cur_instr ) {
        cur_instr = my_instr;
        _firstInstr = my_instr;
      }else {
        cur_instr->next = my_instr;
        my_instr->prev = cur_instr;
        cur_instr = my_instr;
      }
      instr = instr->next;
      if( instr == 0 ) {
        _lastInstr = cur_instr;
      }
    }
  }
} 

/**
 * returns true if the item is in the list
 */
bool Vertex::isItemInList(
              const ExpressionVector &list, 
              const Expression &exp ) {

  bool status = false; 
  ExpressionVector::const_iterator it = 0;

  for( it=list.begin(); it!=list.end(); it++ ) {
    if( exp == *it ) {
      status = true;
      break;
    }
  }
  return status;
}

/**
 * Adds the CSE Instruction to the register
 */
void Vertex::addCSEInstr( my_simple_reg *reg ) {
  assert(_firstInstr);
  my_simple_instr* myInstr = createMySimpleInstr();
 
  myInstr->dst = reg;
  myInstr->opcode = _firstInstr->opcode;
  myInstr->oper = _firstInstr->oper;
  myInstr->src1 = _firstInstr->src1;
  myInstr->src2 = _firstInstr->src2;
  myInstr->old_instruction_no = 0;
  myInstr->block_no = _firstInstr->block_no;
 
  _firstInstr->opcode = CPY_OP;
  _firstInstr->oper = -1;
  _firstInstr->src1 = copyMySimpleReg( reg );
  _firstInstr->src2 = 0;
 
  //arrange the linklist
  myInstr->next = _firstInstr;
  myInstr->prev = 0;
  _firstInstr->prev = myInstr;
  _firstInstr = myInstr;
}
 
/**
 * Modifieds the CSE Instruction to the register
 */
void Vertex::modifyCSEInstr( my_simple_reg *reg ) {
  assert(_firstInstr);
  _firstInstr->opcode = CPY_OP;
  if( _firstInstr->src1 ) {
    deleteMySimpleReg(_firstInstr->src1);
    _firstInstr->src1 = 0;
  }
  if( _firstInstr->src2 ) {
    deleteMySimpleReg(_firstInstr->src2);
    _firstInstr->src2 = 0;
  }
  _firstInstr->oper = -1;
  _firstInstr->src1 = reg;
}

/**
 * Adds the PHI instruction at the begining of the block
 */
void Vertex::addPhiInstr( int regNum, int regKind ) {
  my_simple_reg *myReg = createMySimpleReg();
  myReg->num = regNum;
  myReg->kind = regKind;
  
  my_simple_instr *myInstr = createMySimpleInstr();
  myInstr->opcode = PHI_OP;
  //copy the block number from the next instruction
  myInstr->block_no = _block->num;
  myInstr->dst = myReg;

  int predNum = _prevList.size();
  if( predNum > 0 ) {
    myInstr->nargs = predNum;
    myInstr->args = new my_simple_reg*[predNum];
    for(int n = 0; n < predNum; n++) {
      myReg = createMySimpleReg();
      myReg->num = regNum;
      myReg->kind = regKind;
      myInstr->args[n] = myReg;
    }
  }

  //change the label of the next instr if any
  if( 0 != _firstInstr ) {
    if( 0 != _firstInstr->lab ) {
      myInstr->lab = _firstInstr->lab; 
      _firstInstr->lab = 0;
      //delete this instruction if it has no opcode
      if( LABEL_OP ==  _firstInstr->opcode ) {
        my_simple_instr *nextInstr = _firstInstr->next;
        if( 0 != nextInstr ) {
          nextInstr->prev = 0;
        }
        if( _lastInstr == _firstInstr ) {
          _lastInstr = nextInstr;
        }
        deleteMySimpleInstr( _firstInstr );
        _firstInstr = nextInstr;
      }
    }
  }
  //arrange the linklist
  myInstr->next = _firstInstr;
  myInstr->prev = 0;
  if( 0  != _firstInstr) {
    _firstInstr->prev = myInstr;
  }else {
    _lastInstr = myInstr;
  } 
  _firstInstr = myInstr;
}

/**
 * Replace the registers with webs
 */
void Vertex::replaceWithWebs( const WebList &webs ) {

  my_simple_instr *myInstr = _firstInstr;
 
  while(myInstr != 0) {
    if( myInstr->dst) {
      replaceWithWebRegister( myInstr, myInstr->dst, webs, true );
    }
    if( myInstr->src1) {
      replaceWithWebRegister( myInstr, myInstr->src1, webs, false );
    }
    if( myInstr->src2) {
      replaceWithWebRegister( myInstr, myInstr->src2, webs, false );
    }
    if( myInstr->proc) {
      replaceWithWebRegister( myInstr, myInstr->proc, webs, false );
    }

    if( myInstr->nargs > 0 ) {
      for( int i=0; i< myInstr->nargs; i++ ) {
        replaceWithWebRegister( myInstr, myInstr->args[i], webs, false );
      }
    }
    myInstr = myInstr->next;
  }
}

/**
 * replaces the current register with the web register
 */
void Vertex::replaceWithWebRegister(
                             my_simple_instr *myInstr,
                             my_simple_reg *myReg, 
                             const WebList &webs,
                             bool def ) { 

  assert(myInstr);
  assert(myReg);

  IntList list;
   
  WebList::const_iterator it = 0;

  for(it=webs.begin(); it!= webs.end(); it++ ) {
    my_simple_reg *reg = it->getReg();
    if(reg->num == myReg->num ) {

      if(def ) {
        list = it->getDefList();
      }else {
        list = it->getUseList();
      }
      if( isItemInList( list, myInstr->new_instruction_no ) ) {
        my_simple_reg *webReg = it->getWebReg();
        if(webReg){
          myReg->num = webReg->num;
          myReg->kind = webReg->kind;
          break;
        }
      } 
    }
  } 
}

/**
 * Returns the def and use of the vertex. The graph should be
 * a statement graph to get the def and use
 */
void Vertex::getDefAndUse( BitVector &def,
                           BitVector &use ) {

  if( 0 != _firstInstr ) {
    IntSet useSet;

    my_simple_reg *reg = 0;
    reg = _firstInstr->src1;
    if(reg && (reg->num >= 0) ) {
      use.setItem( (reg->num) -1, true );   
      useSet.insert(reg->num);
    }
    reg = _firstInstr->src2;
    if(reg && (reg->num >= 0) ) {
      use.setItem( (reg->num) -1, true );
      useSet.insert(reg->num);
    }
    reg = _firstInstr->proc;
    if(reg && (reg->num >= 0) ) {
      use.setItem( (reg->num) -1, true );   
      useSet.insert(reg->num);
    }
    if( _firstInstr->nargs > 0 ) {
      for(int i=0; i<_firstInstr->nargs;i++) {
        reg = _firstInstr->args[i];
        if(reg && (reg->num >= 0) ) {
          use.setItem( (reg->num) -1, true );   
          useSet.insert(reg->num);
        }
      }
    }
    reg = _firstInstr->dst;
    if(reg && (reg->num >= 0) ) {
      //only consider definition, if there are no uses
      if( useSet.find(reg->num) == useSet.end()  ){ 
        //-1 for the 0th subscript matching
        def.setItem( (reg->num) -1, true );   
      }
    }
  }
}

/**
 * Change the  register type to machine register,
 */
void Vertex::changeToMachineReg( const AdjListVector &adjVector,
                                 my_simple_reg *reg ) {

  if( reg && (SYMBOLIC_REG == reg->kind) ) {
    int pos = AdjList::getPos( adjVector, *reg );
    assert(pos >= 0);
    reg->num = adjVector[pos]._color;
    reg->kind = MACHINE_REG;
  }
}


/**
 * Modifies the code with the machine registers
 */
void Vertex::modifyCodeWithMachineReg( const AdjListVector &adjVector ) {
  my_simple_instr *myInstr = _firstInstr;
  int pos = 0;
  while( myInstr != 0 ) {
    changeToMachineReg( adjVector, myInstr->dst);
    changeToMachineReg( adjVector, myInstr->src1);
    changeToMachineReg( adjVector, myInstr->src2);
    changeToMachineReg( adjVector, myInstr->proc);
    for(int i=0; i<myInstr->nargs; i++){
      changeToMachineReg( adjVector, myInstr->args[i]);
    }
    myInstr = myInstr->next;
  }
}
