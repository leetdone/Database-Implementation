//
//  Config.hpp
//
//  Created by rick gessner on 2/27/22.
//

#ifndef Config_h
#define Config_h
#include <filesystem>
#include <sstream>

#include "Timer.hpp"

namespace ECE141 {

enum class CacheType : int { block = 0,
                             row,
                             view };

struct Config {
  static const char* getDBExtension() { return ".db"; }
  static size_t cacheSize[3];

  static const std::string getStoragePath() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

    return std::filesystem::temp_directory_path().string();

#elif __APPLE__ || defined __linux__ || defined __unix__

    return std::string("/tmp");
    // return std::string("/Users/qianhongbo/Desktop/test");

#endif
  }

  static std::string getDBPath(const std::string& aDBName) {
    std::ostringstream theStream;
    theStream << Config::getStoragePath() << "/" << aDBName << ".db";
    return theStream.str();
  }

  static Timer& getTimer() {
    static Timer theTimer;
    return theTimer;
  }

  // * cachetype: block, row, view...
  static size_t getCacheSize(CacheType aType) {
    return cacheSize[static_cast<int>(aType)];
  }

  static void setCacheSize(CacheType aType, size_t aSize) {
    cacheSize[static_cast<int>(aType)] = aSize;
  }

  // * use this function to configure the cache state
  static bool useCache(CacheType aType) {
    return cacheSize[static_cast<int>(aType)] > 0;
  }

  // * use this function to configure the index state
  static bool useIndex() {
    return true;
  }
};

}  // namespace ECE141

#endif /* Config_h */
