//
//  Entity.cpp
//  PA3
//
//  Created by rick gessner on 3/2/22.
//

#include "Entity.hpp"


#include <algorithm>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "Helpers.hpp"

namespace ECE141 {

Entity::Entity(const std::string& aName) : name(aName), autoincr(0) {}

Entity& Entity::addAttribute(Attribute& anAttribute) {
  attributes.push_back(anAttribute);
  return *this;
}

Entity& Entity::dropAttribute(Attribute& anAttribute) {
    attributes.pop_back();
    return *this;
}

Attribute* Entity::getAttribute(const std::string& aName) const {
  auto it = std::find_if(attributes.begin(), attributes.end(), [&](const Attribute& x) {
    return aName == x.getName();
  });
  return (it != attributes.end()) ? const_cast<Attribute*>(&(*it)) : nullptr;
}

// USE: ask the entity for name of primary key (may not have one...)
const Attribute* Entity::getPrimaryKey() const {
  for (auto& a : attributes) {
    if (a.isPrimaryKey()) {
      return &a;
    }
  }
  return nullptr;
}

StatusResult Entity::encode(std::ostream& anOutput) {
  anOutput << name << " ";
  anOutput << autoincr << " ";
  for (auto& ele : attributes) {
    ele.encode(anOutput);
  }
  return {};
}

StatusResult Entity::decode(std::istream& anInput) {
  anInput >> name;
  anInput >> autoincr;
  while (!anInput.eof() && '\0' != anInput.peek()) {
    Attribute anAttribute;
    anAttribute.decode(anInput);
    attributes.push_back(anAttribute);
    anInput.get();
  }
  return {};
}

StatusResult Entity::validateFieldList(const StringList& aFieldList) {
  // validate whether there exists an invalid field name input
  for (auto& fieldName : aFieldList) {
    if (!getAttribute(fieldName)) {
      return {illegalIdentifier};
    }
  }
  // validate whether there exists enough number of field name;
  // check the duplicate element in the field list
  std::set<std::string> theSet(aFieldList.begin(), aFieldList.end());
  if (theSet.size() == aFieldList.size()) {
    size_t theSize = 0;
    for (auto& attribute : attributes) {
      if (!attribute.isPrimaryKey()) {
        if (!attribute.isNullable()) theSize++;  // count the number of attribute which is not primary key and not nullable
      }
    }
    return theSize <= theSet.size() ? StatusResult{} : StatusResult{identifierExpected};
  }
  return {illegalIdentifier};
}

StatusResult Entity::validateValueList(const StringList& aFieldList, const StringList& aStrList, ValueList& aValueList) {
  std::vector<Value> tmp;
  for (auto& attribute : attributes) {
    if (attribute.isPrimaryKey())
      tmp.push_back(static_cast<int>(getNextAutoIncrement()));
    else {
      auto pos = distance(aFieldList.begin(), find(aFieldList.begin(), aFieldList.end(), attribute.getName()));
      if (static_cast<size_t>(pos) < aFieldList.size()) {
        Value theValue = Helpers::stringToVariant(aStrList[pos], attribute.getType());
        tmp.push_back(theValue);
      } else if (attribute.isNullable()) {
        tmp.push_back(attribute.getDefaultValue());
      } else
        return {valueExpected};
    }
  }
  aValueList.push_back(tmp);
  return {};
}

}  // namespace ECE141
