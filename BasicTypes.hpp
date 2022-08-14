//
//  BasicTypes.hpp
//  RGAssignement1
//
//  Created by rick gessner on 3/9/21.
//

#ifndef BasicTypes_h
#define BasicTypes_h

#include <variant>
#include <string>
#include <vector>
#include <map>
#include <utility>

namespace ECE141 {

using Value = std::variant<int, float, bool, std::string>;
using ValueList = std::vector<std::vector<Value>>;
using StringList = std::vector<std::string>;
using IntList = std::vector<int>;
using KeyValues = std::map<std::string, Value>;
using BlockNumList = std::vector<uint32_t>;

enum class DataTypes {
  no_type='N',  bool_type='B', datetime_type='D',
  float_type='F', int_type='I', varchar_type='V',
};

}
#endif /* BasicTypes_h */
