//
//  LRUCache.hpp
//  Assignment10
//

#ifndef LRUCache_hpp
#define LRUCache_hpp
#include <algorithm>
#include <list>
#include <unordered_map>

namespace ECE141 {

// template <typename KeyT, typename ValueT>
// using CacheIndex = std::unordered_map<KeyT, list<KeyT>::iterator>;

template <typename KeyT, typename ValueT>
class LRUCache {
 public:
  explicit LRUCache(size_t aSize)
      : maxsize(aSize) {
  }

  // * get the current size
  size_t size() const {
    return dq.size();
  } 

  bool contains(const KeyT& key) const {
    return map.count(key);
  }

  ValueT get(const KeyT& key) {
    // move the priority to the front
    dq.splice(dq.begin(), dq, index[key]);
    return map[key];
  }

  void erase(const KeyT& key) {
    dq.erase(index[key]);
    index.erase(key);
    map.erase(key);
  }

  // * add a new record to cache
  void refer(const KeyT& key, const ValueT& value) {
    if (!contains(key)) { 
      // check whether cache is full
      if (dq.size() == maxsize) {
        KeyT thelast = dq.back(); // preserve the last element (blockNum)
        index.erase(thelast);
        map.erase(thelast);
        dq.pop_back();  // pop out the last element
      }
    } else {
      // exist in cache!
      dq.erase(index[key]);
    }
    dq.push_front(key);
    index[key] = dq.begin();
    map[key] = value;
  }

 protected:
  size_t                    maxsize;
  std::list<KeyT>           dq;
  std::unordered_map<KeyT, typename std::list<KeyT>::iterator>  index;
  std::unordered_map<KeyT, ValueT> map;
};

}  // namespace ECE141

#endif /* LRUCache_hpp */