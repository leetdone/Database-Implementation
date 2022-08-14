
//
//  TabularView.hpp
//
//  Created by rick gessner on 4/1/22.
//  Copyright © 2022 rick gessner. All rights reserved.

#ifndef TabularView_h
#define TabularView_h

#include <iostream>

#include "Helpers.hpp"
#include "Row.hpp"
#include "View.hpp"

namespace ECE141 {

class TabularView : public View {
 public:
  explicit TabularView(RowCollection& aRowCol);
  TabularView& setDump();
  TabularView& setWidth(const IntList& aVector);
  TabularView& setWidth(IntList&& aVector);
  TabularView& setKeys(const StringList& aVector);
  TabularView& setKeys(StringList&& aVector);
  TabularView& setLimit(const size_t& aLimit);
  TabularView& sortRows(const std::string& aStr);
  bool show(std::ostream& output) override;

 protected:
  TabularView& showSeparator(std::ostream& output);
  TabularView& showHeader(std::ostream& output);
  TabularView& showRow(Row& aRow, std::ostream& output);

  bool isDump;
  RowCollection& rows;
  IntList widths;
  StringList keys;
  size_t limit;
};

}  // namespace ECE141

#endif /* TabularView_h */
