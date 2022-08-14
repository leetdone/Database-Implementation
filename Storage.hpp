//
//  Storage.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <stack>
#include <string>

#include "BlockIO.hpp"
#include "Errors.hpp"

namespace ECE141 {

struct CreateDB {};
struct OpenDB {};

const uint32_t kNewBlock = std::numeric_limits<std::uint32_t>::max();

class Storable {
 public:
  virtual StatusResult encode(std::ostream& anOutput) = 0;
  virtual StatusResult decode(std::istream& anInput) = 0;
};

struct StorageInfo {
  StorageInfo(size_t anId = 0, BlockType aType = BlockType::data_block, size_t aRefId = 0, size_t theSize = 0, uint32_t aStartPos = kNewBlock)
      : id(anId), type(aType), refId(aRefId), size(theSize), start(aStartPos) {
  }
  size_t id;  //
  BlockType type;
  size_t refId;    //
  size_t size;     //
  uint32_t start;  // block #
};

using BlockList = std::deque<uint32_t>;
using BlockVisitor = std::function<bool(const StorageInfo&, const uint32_t&, std::stringstream&)>;

struct BlockIterator {
  virtual bool each(BlockVisitor) = 0;
};

class Storage : public BlockIO, public BlockIterator, public Storable {
 public:
  Storage(const std::string& aPath, const std::ios::openmode& aMode);
  ~Storage();

  StatusResult save(std::iostream& aStream, StorageInfo& anInfo);
  StatusResult load(std::iostream& aStream, uint32_t aStartBlockNum, StorageInfo& anInfo);
  bool         each(BlockVisitor aVisitor) override;
  StatusResult markBlockAsFree(uint32_t aPos);
  StatusResult releaseBlocks(uint32_t aPos, bool aInclusive = false);
  uint32_t     getFreeBlock();
  uint32_t     getNextFreeBlock();

  virtual StatusResult encode(std::ostream& anOutput) override;
  virtual StatusResult decode(std::istream& anInput) override;

 protected:
  BlockList available;
};

}  // namespace ECE141

#endif /* Storage_hpp */
