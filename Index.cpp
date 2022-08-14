#include "Index.hpp"

#include <sstream>
#include <utility>

#include "Helpers.hpp"

namespace ECE141 {

Index::Index(Storage &aStorage, const std::string &aName, uint32_t aBlockNum,
             size_t aHash, IndexType aType)
    : storage(aStorage),
      name(aName),
      blockNum(aBlockNum),
      entityId(aHash),
      type(aType),
      changed(false) {
}

Index::~Index() {
  if (changed) {
    std::stringstream theStream;
    encode(theStream);
    StorageInfo theInfo{entityId,
                        0 == blockNum ? BlockType::meta_block : BlockType::index_block,
                        0, Helpers::getStreamSize(theStream), blockNum};
    storage.save(theStream, theInfo);
  }
}

Index::ValueProxy::ValueProxy(Index &anIndex, int aKey)
    : index(anIndex), key(aKey), type(IndexType::intKey) {
}

Index::ValueProxy::ValueProxy(Index &anIndex, const std::string &aKey)
    : index(anIndex), key(aKey), type(IndexType::strKey) {}

Index::ValueProxy &Index::ValueProxy::operator=(uint32_t aValue) {
  index.setKeyValue(key, aValue);
  return *this;
}

Index::ValueProxy::operator IntOpt() {
  return index.valueAt(key);
}

Index::ValueProxy Index::operator[](const std::string &aKey) {
  return ValueProxy(*this, aKey);
}

Index::ValueProxy Index::operator[](uint32_t aKey) {
  return ValueProxy(*this, aKey);
}

uint32_t Index::getBlockNum() const {
  return blockNum;
}

size_t Index::getSize() const {
  return data.size();
}

std::string Index::getName() const {
  return name;
}

Index &Index::setBlockNum(uint32_t aBlockNum) {
  blockNum = aBlockNum;
  return *this;
}

bool Index::isChanged() {
  return changed;
}

Index &Index::setChanged(bool aChanged) {
  changed = aChanged;
  return *this;
}

IntOpt Index::valueAt(const Value &aKey) {
  return exists(aKey) ? data[aKey] : (IntOpt)(std::nullopt);
}

bool Index::setKeyValue(const Value &aKey, uint32_t aValue) {
  data[aKey] = aValue;
  return changed = true;  // side-effect indended!
}

StatusResult Index::erase(const Value &aValue) {
  if (exists(aValue)) {
    data.erase(aValue);
    return {};
  }
  return {unknownError};
}

bool Index::exists(const Value &aKey) {
  return data.count(aKey);
}

StatusResult Index::encode(std::ostream &anOutput) {
  anOutput << name << " ";
  anOutput << static_cast<size_t>(type) << " ";
  anOutput << blockNum << " ";  // TODO: may need to be deleted!

  for (auto &[key, value] : data) {
    std::visit(VWriter{anOutput, true}, const_cast<Value &>(key));
    anOutput << " ";
    anOutput << value << " ";
  }

  return {};
}

StatusResult Index::decode(std::istream &anInput) {
  if ('\0' != anInput.peek()) {
    size_t theNum;
    Value theKey;
    uint32_t theBlockNum;
    std::string tmp;

    anInput >> name;
    anInput >> theNum;
    type = static_cast<IndexType>(theNum);
    anInput >> blockNum;
    anInput.get();
    while (!anInput.eof() && '\0' != anInput.peek()) {
      anInput >> tmp;
      IndexType::intKey == type ? theKey = std::stoi(tmp) : theKey = tmp;
      anInput >> theBlockNum;
      data[theKey] = theBlockNum;
      anInput.get();  // skip the space
    }
  }
  return {};
}

// visit blocks associated with index
bool Index::each(BlockVisitor aVisitor) {
  Block theBlock;
  for (auto &pair : data) {
    std::stringstream theStream;
    StorageInfo theInfo;
    if (storage.load(theStream, pair.second, theInfo)) {
      if (!aVisitor(theInfo, pair.second, theStream)) {
        return false;
      }
    }
  }
  return true;
}

// visit index values (key, value)...
bool Index::eachKV(IndexVisitor aCall) {
  for (auto thePair : data) {
    if (!aCall(thePair.first, thePair.second)) {
      return false;
    }
  }
  return true;
}

}  // namespace ECE141
