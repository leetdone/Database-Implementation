#include <vector>
#include <memory>
#include <sstream>
#include "BlockRetriever.hpp"
#include "Database.hpp"
#include "Entity.hpp"
#include "Row.hpp"
#include "Helpers.hpp"

namespace ECE141 {

BlockObserver::BlockObserver(RowCollection& aRows, size_t anEntityId)
: rows(aRows), entityId(anEntityId) {
}

bool BlockObserver::operator()(const StorageInfo& theInfo, const uint32_t& aBlockNum, std::stringstream& aStream) {
  if (entityId == theInfo.id && BlockType::data_block == theInfo.type) {
    std::unique_ptr<Row> theRow = std::make_unique<Row>(entityId);
    theRow->setBlockNumber(aBlockNum);
    theRow->decode(aStream);
    rows.push_back(std::move(theRow));
  }
  return true;
}

BlockRetriver::BlockRetriver() {
}

BlockRetriver& BlockRetriver::loadFrom(Entity* anEntity, RowCollection* aRowCol) {
  requests[anEntity] = aRowCol;
  return *this;
}

using BlockLoaders = std::map<BlockIterator*, std::vector<BlockObserver> >;

StatusResult BlockRetriver::load(Database& aDB) {
  BlockLoaders theLoaders;

  for (auto& [theEntity, theRows]: requests) {
    std::string theName = theEntity->getName();
    size_t theHash = Helpers::hashString(theName.c_str());
    
    if (Index* theIndex = aDB.getIndex(theName)) {
      theLoaders[theIndex].emplace_back(*requests[theEntity], theHash);
    } else {
      theLoaders[aDB.getStorage()].emplace_back(*requests[theEntity], theHash);
    }
  }

  for (auto& [theIterator, theVector]: theLoaders) {
    for (const auto& theObserver: theVector) {
      if (!theIterator->each(theObserver)) break;
    }
  }
  return {};
}

}  // namespace ECE141
