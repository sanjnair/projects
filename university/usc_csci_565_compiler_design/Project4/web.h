#if !defined __web_H__
#define __web_H__

#include "mysimple.h"
#include "prjsupport.h"

/**
 * Encapsulates the web  of the node
 */
class Web  {
public:

  /** 
   * Constructs a Web  with register
   */
  Web( my_simple_reg *reg,
       IntList defs,
       IntList uses );
   
  /**
   * Copy C'tor
   */
  Web(const Web &rhs);
 
  /**
   * Assignment operator
   */
  Web operator= (const Web &rhs);

  /**
   * Destroys the web 
   */
  ~Web ();

  /**
   * less than operator
   */
  inline bool operator<( const Web& y) const {
    int def1 = -1; 
    int def2 = -1;

    IntList list = _defList;
    IntList::const_iterator it = list.begin();
    if( it != list.end() ) {
      def1 = *it;  
    }
    list = y.getDefList();
    it = list.begin();
    if( it != list.end() ) {
      def2 = *it;  
    }
    return def1 < def2;
  }

  /**
   * returns the definition
   */
  IntList getDefList() const;

  /**
   * Sets the def list
   */
  void setDefList( IntList &list );

  /**
   * Returns the use list
   */
  IntList getUseList() const;

  /**
   * Sets the use list
   */
  void setUseList( IntList &list );

  /**
   * Gets the register
   */
  my_simple_reg *getReg() const;

  /** 
   * Returns the web reg
   */
  my_simple_reg* getWebReg() const;

private:
  IntList _defList;
  IntList _useList;
  my_simple_reg *_reg;
  my_simple_reg *_webReg;
};

typedef std::list<Web>WebList;
#endif //__web_H__
