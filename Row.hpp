//
//  Row.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.


#ifndef Row_hpp
#define Row_hpp

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Storage.hpp"
#include "BasicTypes.hpp"

//feel free to use this, or create your own version...

namespace ECE141 {

class Row : public Storable {
public:
  Row(size_t entityId=0);
  Row(const Row &aRow);
  // Row(const Attribute &anAttribute); //maybe?
  virtual ~Row() = default;
  Row &operator=(const Row &aRow);
  bool operator==(const Row &aCopy) const;
  
  Row&        set(const std::string &aKey, const Value &aValue);
  Row&        setBlockNumber(uint32_t aNum);
  size_t      getEntityId();
  KeyValues&  getData();
  uint32_t    getBlockNumber();

  virtual StatusResult  encode(std::ostream &anOutput) override;
  virtual StatusResult  decode(std::istream &anInput) override;
protected:
  size_t         entityId;    //hash value of entity
  KeyValues      data;
  uint32_t       blockNumber = 0;
};

using RowCollection = std::vector<std::unique_ptr<Row>>;

}
#endif /* Row_hpp */
