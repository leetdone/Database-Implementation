#include "Joins.hpp"

namespace ECE141 {

Join::Join(const std::string& aTable, Keywords aType, const Expression& anExp)
    : table(aTable), joinType(aType), expr(anExp) {}

bool Join::matches(Row* aLRow, Row* aRRow) const {
  const KeyValues& theLMap = aLRow->getData();
  const KeyValues& theRMap = aRRow->getData();

  std::string theLAtt = expr.lhs.name;
  std::string theRAtt = expr.rhs.name;

  if (theLMap.count(theLAtt) && theRMap.count(theRAtt)) {
    if (theLMap.at(theLAtt) == theRMap.at(theRAtt)) {
      return true;
    }
  } else if (theLMap.count(theRAtt) && theRMap.count(theLAtt)) {
    if (theLMap.at(theRAtt) == theRMap.at(theLAtt)) {
      return true;
    }
  }
  return false;
}

Joiner::Joiner(RowCollection& aLeft, RowCollection& aRight)
    : left(aLeft), right(aRight) {
}

StatusResult Joiner::generate(const Join& aJoin, RowCollection& aRowCol) {
  for (auto& theLRow : left) {
    bool flag = false;
    for (auto& theRRow : right) {
      if (aJoin.matches(theLRow.get(), theRRow.get())) {
        KeyValues theLMap(theLRow->getData());
        KeyValues theRMap(theRRow->getData());
        theLMap.merge(theRMap);
        std::unique_ptr<Row> aRow = std::make_unique<Row>();
        aRow->getData().insert(theLMap.begin(), theLMap.end());
        aRowCol.push_back(std::move(aRow));
        flag = true;
      }
    }
    if (!flag) {
      aRowCol.push_back(std::move(theLRow));
    }
  }
  return {};
}

}  // namespace ECE141