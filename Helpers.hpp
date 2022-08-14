//
//  Helpers.hpp
//  Database3
//
//  Created by rick gessner on 4/14/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Helpers_h
#define Helpers_h

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <sstream>

#include "BasicTypes.hpp"
#include "BlockIO.hpp"
#include "keywords.hpp"

namespace ECE141 {

// used by Variant dispatch
struct VWriter {
  explicit VWriter(std::ostream& anOut, bool aFlag = false)
      : out(anOut), flag(aFlag) {}
  void operator()(const int& aVal) { out << aVal; }
  void operator()(const float& aVal) { out << std::setprecision(3) << aVal; }
  void operator()(const bool& aVal) {
    static const StringList tmp = {"false", "true"};
    out << tmp[aVal];
  }
  void operator()(std::string& aVal) {
    // check whether there exists a white space in the string
    if (flag) {
      std::replace(aVal.begin(), aVal.end(), ' ', '$');
    }
    out << aVal;
  }

  std::ostream& out;
  bool flag;  // for encoding we need to replace the white space
};

template <typename T, size_t aSize>
bool in_array(T (&anArray)[aSize], const T& aValue) {
  return std::find(std::begin(anArray), std::end(anArray), aValue);
}

// static std::map<std::string, Operators> gExpressionOps = {
//   {"=",   Operators::equal_op},
//   {"<",   Operators::lt_op},
//   {"<=",  Operators::lte_op},
//   {">",   Operators::gt_op},
//   {">=",  Operators::gte_op},
//   {"!=",  Operators::notequal_op},
//   {"not", Operators::notequal_op},
// };

static std::map<std::string, Operators> gOperators = {
    std::make_pair(".", Operators::dot_op),
    std::make_pair("+", Operators::add_op),
    std::make_pair("-", Operators::subtract_op),
    std::make_pair("*", Operators::multiply_op),
    std::make_pair("/", Operators::divide_op),
    std::make_pair("^", Operators::power_op),
    std::make_pair("%", Operators::mod_op),
    std::make_pair("=", Operators::equal_op),
    std::make_pair("!=", Operators::notequal_op),
    std::make_pair("not", Operators::notequal_op),
    std::make_pair("<", Operators::lt_op),
    std::make_pair("<=", Operators::lte_op),
    std::make_pair(">", Operators::gt_op),
    std::make_pair(">=", Operators::gte_op),
    std::make_pair("and", Operators::and_op),
    std::make_pair("or", Operators::or_op),
    std::make_pair("nor", Operators::nor_op),
    std::make_pair("between", Operators::between_op),
};

static std::map<Operators, Operators> gContraryOperators = {
    std::make_pair(Operators::equal_op, Operators::notequal_op),
    std::make_pair(Operators::lt_op, Operators::gte_op),
    std::make_pair(Operators::gt_op, Operators::lte_op),
    std::make_pair(Operators::gte_op, Operators::lt_op),
    std::make_pair(Operators::lte_op, Operators::gt_op),
};

static std::map<std::string, Logical> gLogicals = {
    std::make_pair("and", Logical::and_op),
    std::make_pair("or", Logical::or_op),
    std::make_pair("not", Logical::not_op),
};

// a list of known functions...
static std::map<std::string, int> gFunctions = {
    std::make_pair("avg", 10),
    std::make_pair("count", 20),
    std::make_pair("max", 30),
    std::make_pair("min", 40)};

// This map binds a keyword string with a Keyword (token)...
static std::map<std::string, Keywords> gDictionary = {
    std::make_pair("add", Keywords::add_kw),
    std::make_pair("all", Keywords::all_kw),
    std::make_pair("alter", Keywords::alter_kw),
    std::make_pair("and", Keywords::and_kw),
    std::make_pair("as", Keywords::as_kw),
    std::make_pair("asc", Keywords::asc_kw),
    std::make_pair("avg", Keywords::avg_kw),
    std::make_pair("auto_increment", Keywords::auto_increment_kw),
    std::make_pair("between", Keywords::between_kw),
    std::make_pair("boolean", Keywords::boolean_kw),
    std::make_pair("by", Keywords::by_kw),
    std::make_pair("change", Keywords::change_kw),
    std::make_pair("changed", Keywords::changed_kw),
    std::make_pair("char", Keywords::char_kw),
    std::make_pair("column", Keywords::column_kw),
    std::make_pair("count", Keywords::count_kw),
    std::make_pair("create", Keywords::create_kw),
    std::make_pair("cross", Keywords::cross_kw),
    std::make_pair("current_date", Keywords::current_date_kw),
    std::make_pair("current_time", Keywords::current_time_kw),
    std::make_pair("current_timestamp", Keywords::current_timestamp_kw),
    std::make_pair("database", Keywords::database_kw),
    std::make_pair("databases", Keywords::databases_kw),
    std::make_pair("datetime", Keywords::datetime_kw),
    std::make_pair("decimal", Keywords::decimal_kw),
    std::make_pair("default", Keywords::default_kw),
    std::make_pair("delete", Keywords::delete_kw),
    std::make_pair("describe", Keywords::describe_kw),
    std::make_pair("distinct", Keywords::distinct_kw),
    std::make_pair("double", Keywords::double_kw),
    std::make_pair("drop", Keywords::drop_kw),
    std::make_pair("dump", Keywords::dump_kw),
    std::make_pair("enum", Keywords::enum_kw),
    std::make_pair("explain", Keywords::explain_kw),
    std::make_pair("false", Keywords::false_kw),
    std::make_pair("float", Keywords::float_kw),
    std::make_pair("foreign", Keywords::foreign_kw),
    std::make_pair("from", Keywords::from_kw),
    std::make_pair("full", Keywords::full_kw),
    std::make_pair("group", Keywords::group_kw),
    std::make_pair("help", Keywords::help_kw),
    std::make_pair("in", Keywords::in_kw),
    std::make_pair("index", Keywords::index_kw),
    std::make_pair("indexes", Keywords::indexes_kw),
    std::make_pair("inner", Keywords::inner_kw),
    std::make_pair("insert", Keywords::insert_kw),
    std::make_pair("int", Keywords::integer_kw),
    std::make_pair("integer", Keywords::integer_kw),
    std::make_pair("into", Keywords::into_kw),
    std::make_pair("join", Keywords::join_kw),
    std::make_pair("key", Keywords::key_kw),
    std::make_pair("last", Keywords::last_kw),
    std::make_pair("left", Keywords::left_kw),
    std::make_pair("like", Keywords::like_kw),
    std::make_pair("limit", Keywords::limit_kw),
    std::make_pair("max", Keywords::max_kw),
    std::make_pair("min", Keywords::min_kw),
    std::make_pair("modify", Keywords::modify_kw),
    std::make_pair("not", Keywords::not_kw),
    std::make_pair("null", Keywords::null_kw),
    std::make_pair("on", Keywords::on_kw),
    std::make_pair("or", Keywords::or_kw),
    std::make_pair("order", Keywords::order_kw),
    std::make_pair("outer", Keywords::outer_kw),
    std::make_pair("primary", Keywords::primary_kw),
    std::make_pair("query", Keywords::query_kw),
    std::make_pair("quit", Keywords::quit_kw),
    std::make_pair("references", Keywords::references_kw),
    std::make_pair("right", Keywords::right_kw),
    std::make_pair("rows", Keywords::rows_kw),
    std::make_pair("run", Keywords::run_kw),
    std::make_pair("script", Keywords::script_kw),
    std::make_pair("select", Keywords::select_kw),
    std::make_pair("self", Keywords::self_kw),
    std::make_pair("set", Keywords::set_kw),
    std::make_pair("show", Keywords::show_kw),
    std::make_pair("sum", Keywords::sum_kw),
    std::make_pair("table", Keywords::table_kw),
    std::make_pair("tables", Keywords::tables_kw),
    std::make_pair("true", Keywords::true_kw),
    std::make_pair("unique", Keywords::unique_kw),
    std::make_pair("update", Keywords::update_kw),
    std::make_pair("use", Keywords::use_kw),
    std::make_pair("values", Keywords::values_kw),
    std::make_pair("varchar", Keywords::varchar_kw),
    std::make_pair("version", Keywords::version_kw),
    std::make_pair("where", Keywords::where_kw)};

static std::map<Keywords, DataTypes> gKeywordTypes = {
    std::make_pair(Keywords::boolean_kw, DataTypes::bool_type),
    std::make_pair(Keywords::datetime_kw, DataTypes::datetime_type),
    std::make_pair(Keywords::float_kw, DataTypes::float_type),
    std::make_pair(Keywords::integer_kw, DataTypes::int_type),
    std::make_pair(Keywords::varchar_kw, DataTypes::varchar_type)};

class Helpers {
 public:
  // be very careful about collisions!
  static size_t hashString(const char* str) {
    uint32_t h{0};
    unsigned char* p;
    const int gMultiplier = 37;
    for (p = (unsigned char*)str; *p != '\0'; p++) {
      h = gMultiplier * h + *p;
    }
    return h;
  }

  static Keywords getKeywordId(const std::string& aKeyword) {
    auto theIter = gDictionary.find(aKeyword);
    if (theIter != gDictionary.end()) {
      return theIter->second;
    }
    return Keywords::unknown_kw;
  }

  static char indexToChar(size_t anIdx) {
    switch (anIdx) {
      case 0:
        return 'I';
      case 1:
        return 'F';
      case 2:
        return 'B';
      case 3:
        return 'V';
      default:
        return 'U';
    }
  }

  // convert from char to keyword...
  static Keywords charToKeyword(char aChar) {
    switch (toupper(aChar)) {
      case 'I':
        return Keywords::integer_kw;
      case 'T':
        return Keywords::datetime_kw;
      case 'B':
        return Keywords::boolean_kw;
      case 'F':
        return Keywords::float_kw;
      case 'V':
        return Keywords::varchar_kw;
      default:
        return Keywords::unknown_kw;
    }
  }

  static std::string blockTypeToString(BlockType aType) {
    switch (aType) {
      case BlockType::meta_block:
        return "Meta";
      case BlockType::entity_block:
        return "Entity";
      case BlockType::data_block:
        return "Data";
      case BlockType::index_block:
        return "Index";
      case BlockType::free_block:
        return "Free";
      case BlockType::available_block:
        return "Available";
      default:
        return "Unknow";
    }
  }

  static std::string dataTypeToString(DataTypes aType, size_t aSize) {
    switch (aType) {
      case DataTypes::bool_type:
        return "bool";
      case DataTypes::datetime_type:
        return "datetime";
      case DataTypes::float_type:
        return "float";
      case DataTypes::int_type:
        return "int";
      case DataTypes::varchar_type:
        return "varchar(" + std::to_string(aSize) + ")";
      default:
        return "none";
    }
  }

  static const char* keywordToString(Keywords aType) {
    switch (aType) {
      case Keywords::boolean_kw:
        return "bool";
      case Keywords::create_kw:
        return "create";
      case Keywords::database_kw:
        return "database";
      case Keywords::databases_kw:
        return "databases";
      case Keywords::datetime_kw:
        return "datetime";
      case Keywords::describe_kw:
        return "describe";
      case Keywords::drop_kw:
        return "drop";
      case Keywords::float_kw:
        return "float";
      case Keywords::integer_kw:
        return "integer";
      case Keywords::show_kw:
        return "show";
      case Keywords::table_kw:
        return "table";
      case Keywords::tables_kw:
        return "tables";
      case Keywords::use_kw:
        return "use";
      case Keywords::varchar_kw:
        return "varchar";
      default:
        return "unknown";
    }
  }

  // USE: ---validate that given keyword is a datatype...
  static bool isDatatype(Keywords aKeyword) {
    switch (aKeyword) {
      case Keywords::char_kw:
      case Keywords::datetime_kw:
      case Keywords::float_kw:
      case Keywords::integer_kw:
      case Keywords::varchar_kw:
        return true;
      default:
        return false;
    }
  }

  static DataTypes stringToDataType(const std::string& aStr) {
    if ("int" == aStr || "integer" == aStr)
      return DataTypes::int_type;
    else if ("float" == aStr)
      return DataTypes::float_type;
    else if ("varchar" == aStr)
      return DataTypes::varchar_type;
    else if ("boolean" == aStr)
      return DataTypes::bool_type;
    else if ("timestamp" == aStr)
      return DataTypes::datetime_type;
    else
      return DataTypes::no_type;
  }

  static bool stringToBool(const std::string& aStr) {
    if ("true" == aStr)
      return true;
    else
      return false;
  };

  static Value stringToVariant(const std::string& aStr, const DataTypes& aType) {
    Value theValue;
    if (DataTypes::bool_type == aType)
      theValue = stringToBool(aStr);
    else if (DataTypes::float_type == aType)
      theValue = std::stof(aStr);
    else if (DataTypes::int_type == aType)
      theValue = std::stoi(aStr);
    else if (DataTypes::varchar_type == aType)
      theValue = aStr;
    return theValue;
  }

  static DataTypes getTypeForKeyword(Keywords aKeyword) {
    return gKeywordTypes.count(aKeyword)
               ? gKeywordTypes[aKeyword]
               : DataTypes::no_type;
  }

  static Operators toOperator(const std::string& aString) {
    auto theIter = gOperators.find(aString);
    if (theIter != gOperators.end()) {
      return theIter->second;
    }
    return Operators::unknown_op;
  }

  static Operators toContraryOperator(Operators anOp) {
    auto theIter = gContraryOperators.find(anOp);
    if (theIter != gContraryOperators.end()) {
      return theIter->second;
    }
    return Operators::unknown_op;
  }

  static Logical toLogical(const std::string& aString) {
    auto theIter = gLogicals.find(aString);
    if (theIter != gLogicals.end()) {
      return theIter->second;
    }
    return Logical::unknown_op;
  }

  static int getFunctionId(const std::string& anIdentifier) {
    auto theIter = gFunctions.find(anIdentifier);
    if (theIter != gFunctions.end()) {
      return theIter->second;
    }
    return 0;
  }

  static bool isNumericKeyword(Keywords aKeyword) {
    static Keywords theTypes[] = {Keywords::decimal_kw, Keywords::double_kw, Keywords::float_kw, Keywords::integer_kw};
    return std::all_of(std::begin(theTypes), std::end(theTypes), [&](Keywords k) {
      return aKeyword == k;
    });
  }

  static size_t getStreamSize(std::stringstream& aStream) {
    aStream.seekg(0, std::ios::end);
    size_t theSize = aStream.tellg();  // compute the size of the information
    aStream.seekg(0, std::ios::beg);
    return theSize;
  }
};

}  // namespace ECE141

#endif /* Helpers_h */
