//
//  BlockIO.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef BlockIO_hpp
#define BlockIO_hpp

#include <functional>
#include <iostream>
#include <fstream>
#include "Errors.hpp"
#include "LRUCache.hpp"

namespace ECE141 {

enum class BlockType {
  data_block = 'D',
  free_block = 'F',
  unknown_block = 'U',
  meta_block = 'M',
  entity_block = 'E',
  index_block = 'I',
  available_block = 'A',
};

// a small header that describes the block...
struct BlockHeader {
  BlockHeader(BlockType aType = BlockType::data_block)
      : type(static_cast<char>(aType)) {}

  BlockHeader(const BlockHeader &aCopy) {
    *this = aCopy;
  }

  void empty() {
    type = static_cast<char>(BlockType::free_block);
  }

  BlockHeader &operator=(const BlockHeader &aCopy) = default;

  char type;          // char version of block type
  uint8_t count = 0;  // how many parts
  uint8_t pos = 0;    // the pos of count
  uint8_t next = 0;   // next block number in count
  size_t hash = 0;    // the hash value of the entity name
};

const size_t kBlockSize = 1024;
const size_t kPayloadSize = kBlockSize - sizeof(BlockHeader);

// block .................
class Block {
 public:
  Block(BlockType aType = BlockType::data_block);
  Block(const Block &aCopy);
  Block &operator=(const Block &aCopy);
  //  StatusResult write(std::ostream &aStream);
  //  bool isDataBlock(size_t aHash) const;

  BlockHeader header;
  char payload[kPayloadSize] = {0};
};

//------------------------------

class BlockIO {
 public:
  BlockIO(const std::string &aPath, const std::ios::openmode &aMode);
  virtual ~BlockIO();

  uint32_t getBlockCount();

  virtual StatusResult readBlock(uint32_t aBlockNumber, Block &aBlock);
  virtual StatusResult writeBlock(uint32_t aBlockNumber, Block &aBlock);

 protected:
  std::fstream              stream;
  LRUCache<uint32_t, Block> cache;
};

}  // namespace ECE141

#endif /* BlockIO_hpp */
