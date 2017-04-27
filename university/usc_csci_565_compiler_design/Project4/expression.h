#if !defined __expression_H__
#define __expression _H__

#include "mysimple.h"
#include "prjsupport.h"

/**
 * Encapsulates the expression of the node
 */
class Expression {
public:

  /** 
   * Constructs a Expression with id and block
   */
  Expression( int oper, 
              my_simple_reg *src1,
              my_simple_reg *src2 );
   

  /**
   * Copy C'tor
   */
  Expression(const Expression& rhs);
 
  /**
   * Assignment operator
   */
  Expression operator= (const Expression &rhs);

  /**
   * Overload the == operator
   */
  bool operator== (const Expression &rhs) const;

  /**
   * Destroys the expression
   */
  ~Expression();

  int _oper;
  my_simple_reg *_src1;
  my_simple_reg *_src2;   
};

typedef std::vector<Expression> ExpressionVector;
#endif //__expression _H__
