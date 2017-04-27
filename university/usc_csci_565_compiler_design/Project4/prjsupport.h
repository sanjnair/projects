#if !defined __PrjSupport_H__
#define __PrjSupport_H__

#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <list>
#include <string>

typedef std::list<int> IntList;
typedef std::map<int,int> IntMap;
typedef std::map<int, IntList> IntListMap;
typedef std::vector<int> IntVector;
typedef std::vector<bool> BoolVector;
typedef std::set<int> IntSet;
typedef std::stack<int> IntStack;
typedef std::queue<int> IntQueue;
typedef std::map<int,IntStack> IntStackMap;
typedef std::vector<BoolVector> BoolMatrix;
typedef std::pair<int,int> IntPair;
typedef std::vector<IntPair> IntPairVector;

#endif //__PrjSupport_H__
