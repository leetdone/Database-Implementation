//
//  TabularView.cpp
//  assignment7_sup
//
//  Created by Maverick on 5/17/22.
//

#include "TabularView.hpp"

namespace ECE141 {

TabularView::TabularView(RowCollection& aRowCol) : isDump(false), rows(aRowCol), limit(std::numeric_limits<size_t>::max()) {}

TabularView& TabularView::setDump() {
  isDump = true;
  return *this;
}

TabularView& TabularView::setWidth(const IntList& aVector) {
  widths = aVector;
  return *this;
}

TabularView& TabularView::setWidth(IntList&& aVector) {
  widths = aVector;
  return *this;
}

TabularView& TabularView::setKeys(const StringList& aVector) {
  keys = aVector;
  return *this;
}

TabularView& TabularView::setKeys(StringList&& aVector) {
  keys = aVector;
  return *this;
}

TabularView& TabularView::setLimit(const size_t& aLimit) {
  limit = aLimit;
  return *this;
}

TabularView& TabularView::sortRows(const std::string& aStr) {
  sort(rows.begin(), rows.end(), [&](std::unique_ptr<Row>& row1, std::unique_ptr<Row>& row2) {
    KeyValues& theMap1 = row1->getData();
    KeyValues& theMap2 = row2->getData();
    return theMap1[aStr] < theMap2[aStr];
  });
  return *this;
}

bool TabularView::show(std::ostream &output) {
  showSeparator(output).showHeader(output).showSeparator(output);
  size_t theSize = rows.size();
  size_t i;
  for(i = 0; i < theSize; i++) {
    if (i >= limit) break; // limit the row we want to show
    showRow(*rows[i], output);
    if (isDump) showSeparator(output);
  }
  if (!isDump) showSeparator(output);
  showRowsInSet(output, i).addTimeStamp(output);
  return true;
}

TabularView& TabularView::showSeparator(std::ostream &output) {
  for (auto aWidth: widths) {
    output << std::left
    << std::setfill('-')
    << std::setw(aWidth)
    << "+";
  }
  output << "+\n";
  return *this;
}

TabularView& TabularView::showHeader(std::ostream &output) {
  size_t theSize = keys.size();
  for (size_t i = 0; i < theSize; i++) {
    output << "| "
    << std::left
    << std::setfill(' ')
    << std::setw(widths[i] - 2)
    << keys[i];
  }
  output << "| \n";
  return *this;
}

TabularView& TabularView::showRow(Row &aRow, std::ostream &output) {
  KeyValues theMap = aRow.getData();
  size_t theSize = keys.size();
  for (size_t i = 0; i < theSize; i++) {
    output << "| "
    << std::left
    << std::setfill(' ')
    << std::setw(widths[i] - 2);
    if (theMap.count(keys[i])) {
      std::visit(VWriter{output}, theMap[keys[i]]);
    } else {
      output << "NULL";
    }
  }
  output << "| \n";
  return *this;
}

}
