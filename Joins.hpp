//
//  Join.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/21/21.
//

#ifndef Join_h
#define Join_h

#include <string>

#include "Filters.hpp"
#include "Row.hpp"
#include "keywords.hpp"

namespace ECE141 {

class Join {
 public:
  Join(const std::string& aTable, Keywords aType, const Expression& anExp);

  bool matches(Row* aLRow, Row* aRRow) const;

  std::string table;
  Keywords joinType;  // left, right, etc...
  Expression expr;
};

using JoinList = std::vector<Join>;

class Joiner {
 public:
  Joiner(RowCollection& aLeft, RowCollection& aRight);
  StatusResult generate(const Join& aJoin, RowCollection& aRowCol);

 protected:
  RowCollection& left;
  RowCollection& right;
};

}  // namespace ECE141

#endif /* Join_h */
