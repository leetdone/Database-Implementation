//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <iostream>

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "Entity.hpp"
#include "Index.hpp"
#include "Row.hpp"
#include "Storage.hpp"

namespace ECE141 {

class DBQuery;

using EntityMap = std::map<std::string, std::unique_ptr<Entity> >;

class Database {
 public:
  Database(const std::string& aPath, CreateDB);
  Database(const std::string& aPath, OpenDB);
  virtual ~Database() = default;

  StatusResult loadEntityIndex();
  StatusResult loadIndexIndex();
  StatusResult loadAvailableBlock();
  Entity*      getEntity(const std::string& aName);
  Index*       getIndex(const std::string& aName);
  Storage*     getStorage() { return &storage; }
  StatusResult dump(std::ostream& anOutput);
  // function used by SQL statement
  StatusResult createTable(const Entity& anEntity, std::ostream& anOutput);
  StatusResult describeTable(const std::string& aName, std::ostream& anOutput);
  StatusResult dropTable(const std::string& aName, std::ostream& anOutput);
  StatusResult showTables(std::ostream& anOutput);
  // function used by insert and select statement
  StatusResult insertRows(const ValueList& aValueList, std::string aName, std::ostream& anOutput);
  StatusResult selectRows(const DBQuery& aQuery, RowCollection& aRowCol);
  StatusResult showQuery(const DBQuery& aQuery, std::ostream& anOutput);
  // function used by update and delete statement
  StatusResult deleteRows(const DBQuery& aQuery, std::ostream& anOutput);
  StatusResult updateRows(const DBQuery& aQuery, const KeyValues& aMap, std::ostream& anOutput);
  // function used by show index
  StatusResult showIndexes(std::ostream& anOutput);
  StatusResult showIndexFrom(const std::string& anEName, const std::string& aPKName, std::ostream& anOutput);
  StatusResult alertAdd(const std::string& anEName, const std::string& aPKName, std::ostream& anOutput);// I need to add the chosen attributes to the chosen entity in this function
  StatusResult alertDrop(const std::string& anEName, const std::string& aPKName, std::ostream& anOutput);//drop the chosen attributes to the chosen entity in this function
 protected:
  StatusResult loadEntity(const std::string& aName);
  StatusResult loadIndex(const std::string& aName);

  std::string   name;
  Storage       storage;
  EntityMap     entities;
  Index         eIndex;
  IndexMap      indexes;
  Index         iIndex;
};
}  // namespace ECE141
#endif /* Database_hpp */
