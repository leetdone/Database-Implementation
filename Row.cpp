//
//  Row.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#include "Row.hpp"

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "Helpers.hpp"

namespace ECE141 {

Row::Row(size_t entityId)
    : entityId(entityId) {}

Row::Row(const Row &aRow) {
  *this = aRow;
}

Row &Row::operator=(const Row &aRow) {
  this->entityId = aRow.entityId;
  this->data = aRow.data;
  return *this;
}

bool Row::operator==(const Row &aCopy) const {
  return this->entityId == aCopy.entityId && this->data == aCopy.data;
}

// STUDENT: What other methods do you require?
Row &Row::set(const std::string &aKey, const Value &aValue) {
  data[aKey] = aValue;
  return *this;
}

Row &Row::setBlockNumber(uint32_t aNum) {
  blockNumber = aNum;
  return *this;
}

size_t Row::getEntityId() {
  return entityId;
}

KeyValues &Row::getData() {
  return data;
}

uint32_t Row::getBlockNumber() {
  return blockNumber;
}

StatusResult Row::encode(std::ostream &anOutput) {
  for (auto &pair : data) {
    anOutput << pair.first << " ";
    anOutput << Helpers::indexToChar(pair.second.index()) << " ";
    std::visit(VWriter{anOutput, true}, pair.second);
    anOutput << " ";
  }
  return {};
}

StatusResult Row::decode(std::istream &anInput) {
  std::string theNameStr, theValueStr;
  char theChar;
  while (!anInput.eof() && '\0' != anInput.peek()) {
    anInput >> theNameStr;
    anInput >> theChar;
    anInput >> theValueStr;
    std::replace(theValueStr.begin(), theValueStr.end(), '$', ' ');
    data[theNameStr] = Helpers::stringToVariant(theValueStr, static_cast<DataTypes>(theChar));
    anInput.get();
  }
  return {};
}

}  // namespace ECE141
