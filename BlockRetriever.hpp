#include <map>
#include "Row.hpp"

namespace ECE141 {

class Database;
class Entity;

struct BlockObserver {
  explicit BlockObserver(RowCollection& aRows, size_t anEntityId = 0);

  bool operator()(const StorageInfo& theInfo, const uint32_t& aBlockNum, std::stringstream& aStream);
  
  RowCollection& rows;
  size_t         entityId;
};

using BlockRequests = std::map<Entity*, RowCollection*>;

class BlockRetriver {
 public:
  BlockRetriver();

  BlockRetriver& loadFrom(Entity* anEntity, RowCollection* aRowCol);
  StatusResult load(Database& aDB);

 protected:
  BlockRequests requests;
};

}  // namespace ECE141
