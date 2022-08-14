//
//  Index.hpp
//  RGAssignment3
//
//  Created by rick gessner on 4/2/21.
//

#ifndef Index_hpp
#define Index_hpp

#include <functional>
#include <map>
#include <memory>
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "Storage.hpp"

namespace ECE141 {

enum class IndexType { intKey = 0,
                       strKey };

using IndexVisitor = std::function<bool(const Value &, uint32_t)>;
using IntOpt = std::optional<uint32_t>;

struct Index : public Storable, BlockIterator {
  Index(Storage &aStorage, const std::string& aName, uint32_t aBlockNum = 0, size_t aHash = 0, IndexType aType = IndexType::intKey);
  // explicit Index(Storage &aStorage);
  virtual  ~Index();

  class ValueProxy {
   public:
    Index &index;
    Value key;
    IndexType type;

    ValueProxy(Index &anIndex, int aKey);
    ValueProxy(Index &anIndex, const std::string &aKey);
    ValueProxy &operator=(uint32_t aValue);
    operator IntOpt();
  };

  ValueProxy      operator[](const std::string &aKey);
  ValueProxy      operator[](uint32_t aKey);
  bool            isChanged();
  uint32_t        getBlockNum() const;
  size_t          getSize() const;
  std::string     getName() const;
  Index&          setBlockNum(uint32_t aBlockNum);
  Index&          setChanged(bool aChanged);
  IntOpt          valueAt(const Value &aKey);
  bool            setKeyValue(const Value &aKey, uint32_t aValue);
  StatusResult    erase(const Value &aValue);
  bool            exists(const Value &aKey);
  // visit blocks associated with index
  bool            each(BlockVisitor aVisitor) override;
  // visit index values (key, value)...
  bool            eachKV(IndexVisitor aCall);

  StatusResult    encode(std::ostream &anOutput) override;
  StatusResult    decode(std::istream &anInput) override;

 protected:
  Storage&                    storage;
  std::map<Value, uint32_t>   data;
  std::string                 name;      // attribute name
  uint32_t                    blockNum;  // where index storage begins
  size_t                      entityId;
  IndexType                   type;
  bool                        changed;
  
  
};  // index

using IndexMap = std::map<std::string, std::unique_ptr<Index> >;

}  // namespace ECE141

#endif /* Index_hpp */
