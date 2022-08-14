//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include <iostream>
#include "Attribute.hpp"
#include "Helpers.hpp"

namespace ECE141 {
Attribute::Attribute(DataTypes aType)
: type(aType), size(0), autoIncrement(0), primary(0), nullable{1}, status{false}, value(std::string("NULL")) {
}

Attribute::Attribute(const std::string& aName, DataTypes aType, uint32_t aSize)
: name(aName), type(aType), size(aSize), autoIncrement(0), primary(0), nullable{1}, status(false), value(std::string("NULL")) {
}

Attribute& Attribute::setName(const std::string &aName)  {
  name = aName;
  return *this;
}

Attribute& Attribute::setDataType(DataTypes aType) {
  type = aType;
  return *this;
}

Attribute& Attribute::setSize(int aSize) {
  size=aSize;
  return *this;
}

Attribute& Attribute::setAutoIncrement(bool anAuto) {
  autoIncrement=anAuto;
  return *this;
}

Attribute& Attribute::setPrimaryKey(bool aPrimary) {
  primary=aPrimary;
  return *this;
}

Attribute& Attribute::setNullable(bool aNullable) {
  nullable=aNullable;
  return *this;
}

Attribute& Attribute::setDefaultValue(Value& aValue) {
  status = true;
  value = aValue;
  return *this;
}

bool Attribute::isValid() {
  return true; // TODO: remove?
}

Value Attribute::toValue(const std::string &aValue) const {
  return Value{aValue};
}

StatusResult Attribute::encode(std::ostream &anOutput) {
  anOutput << name << ' ';
  anOutput << static_cast<char>(type) << ' ';
  anOutput << size << ' ';
  anOutput << nullable << ' ';
  anOutput << primary << ' ';
  anOutput << autoIncrement << ' ';
  std::visit(VWriter{anOutput}, value);
  anOutput << ' ';
  return {};
}

StatusResult Attribute::decode(std::istream &anInput) {
  char aChar;
  uint16_t aSize, aNull, aPrimary, anInc;
  std::string aStr;
  anInput >> name >> aChar >> aSize >> aNull >> aPrimary >> anInc >> aStr;
  type = static_cast<DataTypes>(aChar);
  size = aSize;
  nullable = aNull;
  primary = aPrimary;
  autoIncrement = aPrimary;
  if ("NULL" != aStr) value = Helpers::stringToVariant(aStr, type);
  return {};
}

}
