//
//  DBQuery.hpp
//  assignment6_sup
//
//  Created by Maverick on 5/7/22.
//

#ifndef DBQuery_hpp
#define DBQuery_hpp

#include <limits>

#include "Filters.hpp"
#include "Joins.hpp"

namespace ECE141 {

class Row;

class DBQuery {
public:
  DBQuery();
  ~DBQuery();
//  DBQuery(const DBQuery& aCopy);
//  DBQuery& operator=(const DBQuery& aCopy);
  
  StringList        getFieldList() const {return list;}
  std::string       getTableName() const {return tbName;}
  Filters&          getFilter() {return filter;}
  std::string       getOrderBy() const {return orderby;}
  size_t            getLimit() const {return limit;}
  const JoinList&   getJoins() const { return joins;}
  DBQuery&          setFieldList(const StringList& aList);
  DBQuery&          setTableName(const std::string& aName);
  DBQuery&          setOrderBy(const std::string& aStr);
  DBQuery&          setLimit(const size_t& aNum);
  DBQuery&          addJoin(const std::string& aName, Keywords aJoinType, Expression anExpr);
  bool              matches(Row& aRow) const;
protected:
  StringList    list;
  std::string   tbName;
  Filters       filter;
  std::string   orderby;
  size_t        limit;
  JoinList      joins;
};
}

#endif /* DBQuery_hpp */
