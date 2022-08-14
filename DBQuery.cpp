//
//  DBQuery.cpp
//  assignment6_sup
//
//  Created by Maverick on 5/7/22.
//

#include "DBQuery.hpp"
#include "Helpers.hpp"
#include "Row.hpp"

namespace ECE141 {

DBQuery::DBQuery()
: tbName(""), orderby(""), limit(std::numeric_limits<size_t>::max()) {}

DBQuery::~DBQuery() {}

//DBQuery::DBQuery(const DBQuery& aCopy) {
//  *this = aCopy;
//}
//
//DBQuery& DBQuery::operator=(const DBQuery& aCopy) {
//  list = aCopy.list;
//  tbName = aCopy.tbName;
//  filter = aCopy.filter;
//  orderby = aCopy.orderby;
//  limit = aCopy.limit;
//  return *this;
//}

DBQuery& DBQuery::setFieldList(const StringList& aList) {
  list = aList;
  return *this;
}

DBQuery& DBQuery::setTableName(const std::string& aName) {
  tbName = aName;
  return *this;
}

DBQuery& DBQuery::setOrderBy(const std::string& aStr) {
  orderby = aStr;
  return *this;
}

DBQuery& DBQuery::setLimit(const size_t& aNum) {
  limit = aNum;
  return *this;
}

DBQuery& DBQuery::addJoin(const std::string& aName, Keywords aJoinType, Expression anExpr) {
  joins.emplace_back(aName, aJoinType, anExpr);
  return *this;
}

bool DBQuery::matches(Row& aRow) const {
  size_t theHash = Helpers::hashString(tbName.c_str());
  if (theHash == aRow.getEntityId()) {
    return filter.matches(aRow.getData());
  }
  return false;
}

}
