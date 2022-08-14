//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.

#include "Storage.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <sstream>

#include "BlockRetriever.hpp"
#include "Config.hpp"
#include "Helpers.hpp"

namespace ECE141 {

Storage::Storage(const std::string& aPath, const std::ios::openmode& aMode)
    : BlockIO(aPath, aMode) {}

Storage::~Storage() {
  std::stringstream theStream;
  encode(theStream);
  StorageInfo theInfo{0, BlockType::available_block, 0, Helpers::getStreamSize(theStream), 2};
  save(theStream, theInfo);
}

StatusResult Storage::save(std::iostream& aStream, StorageInfo& anInfo) {
  StatusResult theResult;
  uint32_t theStartNum = kNewBlock == anInfo.start ? getFreeBlock() : anInfo.start;
  anInfo.start = theStartNum;
  theResult.value = theStartNum;
  size_t theCount = anInfo.size / kPayloadSize + 1;
  size_t thePart = 0;
  Block theBlock;
  // add these two new variable to handle multiple blocks case
  bool flag = false;
  uint32_t theNextBlockNum;
  while (theResult && (anInfo.size > 0)) {
    size_t theBufSize = std::min(anInfo.size, kPayloadSize);
    std::memset(&theBlock.payload, 0, kPayloadSize);
    aStream.read(reinterpret_cast<char*>(&theBlock.payload), theBufSize);
    anInfo.size -= theBufSize;
    theBlock.header.type = static_cast<char>(anInfo.type);
    theBlock.header.count = theCount;
    theBlock.header.pos = ++thePart;
    theBlock.header.next = 0;
    if (anInfo.size) {
      theNextBlockNum = getNextFreeBlock();
      theBlock.header.next = theNextBlockNum;
      flag = true;
    }
    theBlock.header.hash = anInfo.id;
    // TODO: Not sure whether this version is correct
    if (!flag) {
      theResult = writeBlock(theStartNum, theBlock);
    } else {
      theResult = writeBlock(theNextBlockNum, theBlock);
    }

  }
  return theResult;
}

StatusResult Storage::load(std::iostream& aStream, uint32_t aStartBlockNum, StorageInfo& anInfo) {
  StatusResult theResult;
  if (aStartBlockNum < getBlockCount()) {
    Block theBlock;
    while ((theResult = readBlock(aStartBlockNum, theBlock))) {
      anInfo.type = static_cast<BlockType>(theBlock.header.type);
      anInfo.id = theBlock.header.hash;
      aStream.write(reinterpret_cast<char*>(&theBlock.payload), kPayloadSize);
      if (!(theBlock.header.next)) break;
    }
  } else
    theResult = StatusResult{seekError};
  return theResult;
}

bool Storage::each(BlockVisitor aVisitor) {
  Block theBlock;
  size_t theCount = getBlockCount();
  for (uint32_t i = 0; i < theCount; i++) {
    std::stringstream theStream;
    StorageInfo theInfo;
    load(theStream, i, theInfo);
    if (!aVisitor(theInfo, i, theStream)) {
      return false;
    }
  }
  return true;
}

StatusResult Storage::markBlockAsFree(uint32_t aPos) {
  // find the block -> set the type in block header to free -> write it back
  // -> put the pos to the available list...
  Block theBlock;
  if (readBlock(aPos, theBlock)) {
    theBlock.header.type = static_cast<char>(BlockType::free_block);
    theBlock.header.hash = 0;
    if (writeBlock(aPos, theBlock)) {
      available.push_back(aPos);
      return StatusResult{};
    }
  }
  return StatusResult{seekError};
}

StatusResult Storage::releaseBlocks(uint32_t aPos, bool aInclusive) {
  StatusResult theResult;
  Block theBlock;
  // if include current block, release it and add 1 to position
  if (aInclusive) markBlockAsFree(aPos++);
  while ((theResult = readBlock(aPos, theBlock))) {
    markBlockAsFree(aPos);
    if (!(aPos = theBlock.header.next)) break;
  }
  return theResult;
}

uint32_t Storage::getFreeBlock() {
  // first use available block
  if (available.size()) {
    auto temp = available.front();
    available.pop_front();
    return temp;
  }
  // if no available block, return the next block number...
  return getBlockCount();
}

uint32_t Storage::getNextFreeBlock() {
  // first use available block
  if (available.size()) {
    auto temp = available.front();
    available.pop_front();
    return temp;
  }
  // if no available block, return the next block number...
  return getBlockCount() + 1;
}

StatusResult Storage::encode(std::ostream& anOutput) {
  // encode the available block number to stream
  for (auto& theBlockNum : available) {
    anOutput << theBlockNum << " ";
  }
  return {};
}

StatusResult Storage::decode(std::istream& anInput) {
  // decode the available block number
  uint32_t theBlockNum;
  while (!anInput.eof() && '\0' != anInput.peek()) {
    anInput >> theBlockNum;
    available.push_back(theBlockNum);
    anInput.get();  // skip the space
  }
  return {};
}

}  // namespace ECE141
