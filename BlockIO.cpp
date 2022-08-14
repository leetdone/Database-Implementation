//
//  BlockIO.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21. 
//

#include "BlockIO.hpp"

#include <cstring>

#include "Config.hpp"

namespace ECE141 {

Block::Block([[maybe_unused]] BlockType aType) {}

Block::Block(const Block &aCopy) {
  *this = aCopy;
}

Block &Block::operator=(const Block &aCopy) {
  std::memcpy(payload, aCopy.payload, kPayloadSize);
  header = aCopy.header;
  return *this;
}

// StatusResult Block::write(std::ostream &aStream) {
//   return {noError};
// }

//-------------------- - ------------------------------

BlockIO::BlockIO(const std::string &aPath, const std::ios::openmode &aMode)
    : stream(aPath, aMode), cache(Config::getCacheSize(CacheType::block)) {}

BlockIO::~BlockIO() {
  stream.close();
}

// USE: write data a given block (after seek) ---------------------------------------
StatusResult BlockIO::writeBlock(uint32_t aBlockNum, Block &aBlock) {
  if (stream) {
    stream.seekg(aBlockNum * kBlockSize, std::ios::beg);
    stream.write(reinterpret_cast<char *>(&aBlock), kBlockSize);
    if (Config::useCache(CacheType::block)) {
      if (cache.contains(aBlockNum)) {
        cache.erase(aBlockNum);
      }
    }
    return {};
  }

  return {writeError};
}

// USE: write data a given block (after seek) ---------------------------------------
StatusResult BlockIO::readBlock(uint32_t aBlockNumber, Block &aBlock) {
  if (Config::useCache(CacheType::block)) {
    if (stream) {
      stream.seekg(aBlockNumber * kBlockSize, std::ios::beg);
      stream.read(reinterpret_cast<char *>(&aBlock), kBlockSize);
      return {};
    } 

    if (cache.contains(aBlockNumber)) {
      aBlock = cache.get(aBlockNumber);
      return {};
    } else {
      if (stream) {
        stream.seekg(aBlockNumber * kBlockSize, std::ios::beg);
        stream.read(reinterpret_cast<char *>(&aBlock), kBlockSize);
        cache.refer(aBlockNumber, aBlock);
        return {};
      }
    }
  } else {
    if (stream) {
      stream.seekg(aBlockNumber * kBlockSize, std::ios::beg);
      stream.read(reinterpret_cast<char *>(&aBlock), kBlockSize);
      return {};
    }
  }

  return {readError};
}

// USE: count blocks in file ---------------------------------------
uint32_t BlockIO::getBlockCount() {
  if (stream) {
    stream.seekg(0, std::ios::end);
    const auto theEnd = stream.tellg();
    stream.seekg(0, std::ios::beg);
    const auto theBeg = stream.tellg();
    size_t length = theEnd - theBeg;
    return static_cast<uint32_t>(length / kBlockSize);
  }
  return 0;  // What should this be?
}

}  // namespace ECE141
