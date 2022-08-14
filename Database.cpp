//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "Database.hpp"

#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "BlockRetriever.hpp"
#include "Config.hpp"
#include "DBQuery.hpp"
#include "Helpers.hpp"
#include "TabularView.hpp"
#include "View.hpp"

namespace ECE141 {

auto OpenNew = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
auto OpenExisting = std::ios::in | std::ios::out | std::ios::binary;

Database::Database(const std::string& aName, CreateDB)
    : name(aName),
      storage(Config::getDBPath(aName), OpenNew),
      eIndex(storage, "M0", 0, 0, IndexType::strKey),
      iIndex(storage, "M1", 1, 0, IndexType::strKey) {
  StorageInfo theInfo;
  std::stringstream theStream;

  // meta header block
  theInfo = {0, BlockType::meta_block, 0, 1, 0};
  storage.save(theStream, theInfo);
  // meta index block
  theInfo = {0, BlockType::meta_block, 0, 1, 1};
  storage.save(theStream, theInfo);
  // available block number
  theInfo = {0, BlockType::available_block, 0, 1, 2};
  storage.save(theStream, theInfo);
}

Database::Database(const std::string& aName, OpenDB)
    : name(aName),
      storage(Config::getDBPath(aName), OpenExisting),
      eIndex(storage, "M0", 0, 0, IndexType::strKey),
      iIndex(storage, "M1", 1, 0, IndexType::strKey) {
}

StatusResult Database::loadEntityIndex() {
  std::stringstream theStream;
  StorageInfo theInfo;
  uint32_t metaBlockZero = 0;

  if (storage.load(theStream, metaBlockZero, theInfo)) {
    if (eIndex.decode(theStream)) {
      return {};
    }
  }

  return {readError};
}

StatusResult Database::loadIndexIndex() {
  std::stringstream theStream;
  StorageInfo theInfo;
  uint32_t metaBlockNumber = 1;

  if (storage.load(theStream, metaBlockNumber, theInfo))
    if (iIndex.decode(theStream)) {
      return {};
    }
  return {readError};
}

StatusResult Database::loadAvailableBlock() {
  std::stringstream theStream;
  StorageInfo theInfo;
  uint32_t availableBlockNumber = 2;

  if (storage.load(theStream, availableBlockNumber, theInfo))
    if (storage.decode(theStream)) {
      return {};
    }
  return {readError};
}

StatusResult Database::loadEntity(const std::string& aName) {
  StatusResult theResult;
  // first check the index map (from Meta block)
  if (eIndex.exists(aName)) {
    // check whether the entity has been load in memory
    if (!entities.count(aName)) {
      std::stringstream theStream;
      StorageInfo theInfo;
      if ((theResult = storage.load(theStream, eIndex.valueAt(aName).value(), theInfo))) {
        std::unique_ptr<Entity> theEntity = std::make_unique<Entity>(aName);
        if ((theResult = theEntity->decode(theStream)))
          entities[aName] = std::move(theEntity);
      }
    }
  } else
    theResult.error = unknownTable;
  return theResult;
}

StatusResult Database::loadIndex(const std::string& aName) {
  StatusResult theResult;
  std::stringstream theStream;
  StorageInfo theInfo;

  // first check the index map (from Meta block)
  if (iIndex.exists(aName)) {
    // check whether the entity has been load in memory
    if (!indexes.count(aName)) {
      if ((theResult = storage.load(theStream, iIndex.valueAt(aName).value(), theInfo))) {
        size_t theHash = Helpers::hashString(aName.c_str());
        std::string theName = entities[aName]->getPrimaryKey()->getName();
        std::unique_ptr<Index> theIndex = std::make_unique<Index>(storage, theName, theInfo.start, theHash);
        if ((theResult = theIndex->decode(theStream)))
          indexes[aName] = std::move(theIndex);
      }
    }
  } else {
    theResult.error = unknownTable;
  }

  return theResult;
}

Entity* Database::getEntity(const std::string& aName) {
  // if the entity is in the memory
  // or the entity is in the entity index -> load it to memory
  if (entities.count(aName)) {
    return entities[aName].get();
  } else if (loadEntity(aName)) {
    return entities[aName].get();
  }
  return nullptr;
}

Index* Database::getIndex(const std::string& aName) {
  // if the index is in the memory
  // or the index is in the entity index -> load it to memory
  if (indexes.count(aName)) {
    return indexes[aName].get();
  } else if (loadIndex(aName)) {
    return indexes[aName].get();
  }
  return nullptr;
}

// USE: Call this to dump the db for debug purposes...
StatusResult Database::dump(std::ostream& anOutput) {
  RowCollection aRowCol;
  // EntityIndexMap& theIndexMap = eIndex.getEntityMap();

  storage.each([&](const StorageInfo& theInfo, const uint32_t& aBlockNum, [[maybe_unused]] std::stringstream& aStream) {
    std::unique_ptr<Row> aRow = std::make_unique<Row>();
    aRow->set("#", static_cast<int>(aBlockNum));
    aRow->set("Type", Helpers::blockTypeToString(theInfo.type));
    aRow->set("Id", static_cast<int>(theInfo.id));
    std::string aStr;
    eIndex.eachKV([&](const Value& aValue, uint32_t aBlockNum) {
      std::string theName = std::get<std::string>(aValue);
      if (theInfo.id == Helpers::hashString(name.c_str())) {
        aStr = name;
      }
      return true;
    });
    aRow->set("Extra", aStr);
    aRowCol.push_back(std::move(aRow));
    return true;
  });

  TabularView theView(aRowCol);
  IntList widths{10, 15, 15, 15};
  StringList keys{"#", "Type", "Id", "Extra"};
  theView.setDump().setKeys(keys).setWidth(widths).show(anOutput);
  return {};
}

StatusResult Database::createTable(const Entity& anEntity, std::ostream& anOutput) {
  StatusResult theResult;
  // first copy the entity and create an unique_ptr
  std::unique_ptr<Entity> theEntity(new Entity(anEntity));
  std::string theName = theEntity->getName();
  size_t theHash = Helpers::hashString(theName.c_str());
  std::stringstream theStream;
  // save the entity to file
  theEntity->encode(theStream);
  StorageInfo theInfo(theHash, BlockType::entity_block, 0, Helpers::getStreamSize(theStream));
  if ((theResult = storage.save(theStream, theInfo))) {
    // save the entity index to file
    eIndex.setKeyValue(theName, theInfo.start);
    // save the row index block to file
    theStream = std::stringstream();  // clear the stream
    // meta header block
    theInfo = {theHash, BlockType::index_block, 0, 1};
    if ((theResult = storage.save(theStream, theInfo))) {
      std::string thePKName = theEntity->getPrimaryKey()->getName();
      std::unique_ptr<Index> theIndex = std::make_unique<Index>(storage, thePKName, theInfo.start, theHash);
      // add the index to memory
      iIndex.setKeyValue(theName, theIndex->getBlockNum());
      indexes[theName] = std::move(theIndex);
      // save the index block number to meta 1 block
      theStream = std::stringstream();  // clear the stream
      iIndex.encode(theStream);
      theInfo = {0, BlockType::meta_block, 0, Helpers::getStreamSize(theStream), 1};
      if ((theResult = storage.save(theStream, theInfo))) {
        // add the entity to memory
        entities[theName] = std::move(theEntity);
      }
    }
    // show result
    ResultView theView(1);
    theView.show(anOutput);
  }
  return theResult;
}

StatusResult Database::describeTable(const std::string& aName, std::ostream& anOutput) {
  static StringList isNullableStr = {"NO", "YES"};
  static StringList isPrimaryKeyStr = {"", "YES"};
  static StringList autoIncrStr = {"", "auto_increment "};
  static StringList primaryKeyStr = {"", "primary key"};

  StatusResult theResult;
  if (Entity* theEntity = getEntity(aName)) {
    auto attibutes = theEntity->getAttributes();
    size_t aHash = Helpers::hashString(aName.c_str());
    RowCollection aRowCol;
    for (const auto& ele : attibutes) {
      std::unique_ptr<Row> aRow = std::make_unique<Row>(aHash);
      aRow->set("Field", ele.getName())
          .set("Type", Helpers::dataTypeToString(ele.getType(), ele.getSize()))
          .set("Null", isNullableStr[ele.isNullable()])
          .set("Key", isPrimaryKeyStr[ele.isPrimaryKey()])
          .set("Default", ele.getDefaultValue())
          .set("Extra", autoIncrStr[ele.isAutoIncrement()] + primaryKeyStr[ele.isPrimaryKey()]);
      aRowCol.push_back(std::move(aRow));
    }
    TabularView aView(aRowCol);
    IntList widths{17, 14, 7, 6, 10, 30};
    StringList keys{"Field", "Type", "Null", "Key", "Default", "Extra"};
    aView.setKeys(keys).setWidth(widths).show(anOutput);
  }
  return theResult;
}

StatusResult Database::dropTable(const std::string& aName, std::ostream& anOutput) {
  // delete the index
  eIndex.erase(aName);
  indexes.erase(aName);
  iIndex.erase(aName);

  size_t theHash = Helpers::hashString(aName.c_str());
  size_t theCount = 0;

  storage.each([&](const StorageInfo& theInfo, const uint32_t& aBlockNum, [[maybe_unused]] std::stringstream& aStream) {
    if (theHash == theInfo.id) {
      if (BlockType::index_block != theInfo.type) theCount++;
      storage.markBlockAsFree(aBlockNum);
    }
    return true;
  });

  ResultView theView(theCount);  // don't count the entity block
  theView.show(anOutput);
  return {};
}

StatusResult Database::showTables(std::ostream& anOutput) {
  RowCollection aRowCol;

  eIndex.eachKV([&](const Value& aValue, uint32_t aBlockNum) {
    std::string theName = std::get<std::string>(aValue);
    size_t theHash = Helpers::hashString(theName.c_str());
    std::unique_ptr<Row> aRow = std::make_unique<Row>(theHash);
    aRow->set("Tables in " + name, theName);
    aRowCol.push_back(std::move(aRow));
    return true;
  });

  TabularView aView(aRowCol);
  IntList widths{27};
  StringList keys{"Tables in " + name};
  aView.setKeys(keys).setWidth(widths).show(anOutput);
  return {};
}

StatusResult Database::insertRows(const ValueList& aValueList, std::string aName, std::ostream& anOutput) {
  StatusResult theResult;
  AttributeList theList = entities[aName]->getAttributes();
  size_t theHashVal = Helpers::hashString(aName.c_str());
  size_t theValueSize = aValueList.size();

  for (size_t i = 0; i < theValueSize; i++) {
    std::unique_ptr<Row> aRow = std::make_unique<Row>(theHashVal);
    Value thePrimaryKey;

    // add value to the row
    size_t theSize = aValueList[i].size();
    for (size_t j = 0; j < theSize; j++) {
      Attribute theAttribute = theList[j];
      aRow->set(theAttribute.getName(), aValueList[i][j]);
      if (theAttribute.isPrimaryKey()) {
        thePrimaryKey = aValueList[i][j];  // preserve the primary key
      }
    }
    // save the row to .db file
    std::stringstream theStream;
    aRow->encode(theStream);
    StorageInfo theInfo(theHashVal, BlockType::data_block, 0, Helpers::getStreamSize(theStream));  // set the storage info
    if ((theResult = storage.save(theStream, theInfo))) {
      // update the index
      if (Index* theIndex = getIndex(aName)) {
        theIndex->setKeyValue(thePrimaryKey, theInfo.start);
      }
      // still need to encode the entity to update the autoincr!
      std::stringstream anoStream;
      entities[aName]->encode(anoStream);
      theInfo = {theHashVal, BlockType::entity_block, 0, Helpers::getStreamSize(anoStream), eIndex.valueAt(aName).value()};
      theResult = storage.save(anoStream, theInfo);
    }
  }
  // show the result
  ResultView theView(aValueList.size());
  theView.show(anOutput);
  return theResult;
}

StatusResult Database::selectRows(const DBQuery& aQuery, RowCollection& aRowCol) {
  StatusResult theResult;
  std::string theName = aQuery.getTableName();
  BlockRetriver theRetriever;

  // check whether we have joins
  const JoinList& theJoins = aQuery.getJoins();
  if (size_t theSize = theJoins.size()) {
    for (size_t i{0}; i < theSize; i++) {
      if (Entity* theLEntity = getEntity(theName)) {
        if (Entity* theREntity = getEntity(theJoins[i].table)) {
          RowCollection theLeft, theRight;

          theRetriever.loadFrom(theLEntity, &theLeft).loadFrom(theREntity, &theRight);
          if ((theResult = theRetriever.load(*this))) {
            Joiner theJoiner(theLeft, theRight);
            theResult = theJoiner.generate(theJoins[i], aRowCol);
          }
        }
      }
    }
  } else {
    RowCollection theRows;

    if (Entity* theEntity = getEntity(theName)) {
      theRetriever.loadFrom(theEntity, &theRows);
      theResult = theRetriever.load(*this);
      for (auto& r : theRows) {
        if (aQuery.matches(*r)) {
          aRowCol.push_back(std::move(r));
        }
      }
    }
  }

  return theResult;
}

StatusResult Database::showQuery(const DBQuery& aQuery, std::ostream& anOutput) {
  StatusResult theResult;
  RowCollection aRowCol;

  if ((theResult = selectRows(aQuery, aRowCol))) {
    TabularView theView(aRowCol);
    // set limit
    theView.setLimit(aQuery.getLimit());
    // set orderby
    std::string theOrder = aQuery.getOrderBy();
    if (!theOrder.empty()) {
      theView.sortRows(theOrder);
    }

    std::string theName = aQuery.getTableName();
    auto attibutes = getEntity(theName)->getAttributes();
    IntList widths;
    StringList keys;
    StringList theFieldList = aQuery.getFieldList();
    if (theFieldList[0] == "*") {
      size_t theSize = attibutes.size();
      IntList tmp(theSize, 20);
      widths = tmp;
      for (size_t j = 0; j < theSize; j++) {
        keys.push_back(attibutes[j].getName());
      }
    } else {
      size_t theSize = theFieldList.size();
      IntList tmp(theSize, 20);
      widths = tmp;
      keys = theFieldList;
    }
    theView.setKeys(keys).setWidth(widths).show(anOutput);
  }
  return theResult;
}

StatusResult Database::deleteRows(const DBQuery& aQuery, std::ostream& anOutput) {
  StatusResult theResult;
  RowCollection aRowCol;

  if ((theResult = selectRows(aQuery, aRowCol))) {
    // mark all the corresponding rows as free block
    for (auto& row : aRowCol) {
      storage.markBlockAsFree(row->getBlockNumber());
    }

    // get the primary key attribute name
    std::string thePKName;
    std::string theTBName = aQuery.getTableName();
    if (Entity* theEntity = getEntity(theTBName)) {
      if (const Attribute* theAttribute = theEntity->getPrimaryKey()) {
        thePKName = theAttribute->getName();
      }
    }

    // check whether exists primary key
    if (!thePKName.empty()) {
      // update the index
      size_t theSize = aRowCol.size();
      for (size_t i{0}; i < theSize; i++) {
        Value theValue = aRowCol[i]->getData()[thePKName];
        getIndex(theTBName)->erase(theValue);
      }
    }
  }
  ResultView theView(aRowCol.size());
  theView.show(anOutput);
  return theResult;
}

StatusResult Database::updateRows(const DBQuery& aQuery, const KeyValues& aMap, std::ostream& anOutput) {
  StatusResult theResult;
  RowCollection aRowCol;

  if ((theResult = selectRows(aQuery, aRowCol))) {
    for (auto& theRow: aRowCol) {
      KeyValues& theMap = theRow->getData();
      // update the selected row
      for (auto& pair : aMap) {
        theMap[pair.first] = pair.second;
      }
      // encode the row to .db file
      std::stringstream theStream;
      if ((theResult = theRow->encode(theStream))) {
        size_t theHash = Helpers::hashString(aQuery.getTableName().c_str());
        StorageInfo theInfo(theHash, BlockType::data_block, 0, Helpers::getStreamSize(theStream), theRow->getBlockNumber());  // set the storage info
        theResult = storage.save(theStream, theInfo);
      }
    }
  }
  ResultView theView(aRowCol.size());
  theView.show(anOutput);
  return theResult;
}

StatusResult Database::showIndexes(std::ostream& anOutput) {
  RowCollection aRowCol;
  // iterate through the row index to set the tabular view
  for (auto& [theName, theIndex] : indexes) {
    size_t theHash = Helpers::hashString(theName.c_str());
    std::unique_ptr<Row> aRow = std::make_unique<Row>(theHash);
    aRow->set("table", theName).set("field(s)", theIndex->getName());
    aRowCol.push_back(std::move(aRow));
  }
  // create the tabular view and show
  TabularView aView(aRowCol);
  IntList widths{20, 20};
  StringList keys{"table", "field(s)"};
  aView.setKeys(keys).setWidth(widths).show(anOutput);

  return {};
}

StatusResult Database::showIndexFrom(const std::string& anEName, const std::string& aPKName, std::ostream& anOutput) {
  RowCollection aRowCol;
  size_t theHash = Helpers::hashString(anEName.c_str());

  if (!loadIndex(anEName)) {
    return {unknownIndex};
  }

  indexes[anEName]->eachKV([&](const Value& aValue, uint32_t aBlockNum) {
    std::unique_ptr<Row> aRow = std::make_unique<Row>(theHash);
    std::stringstream theStream;
    std::visit(VWriter{theStream}, const_cast<Value&>(aValue));
    aRow->set("key", theStream.str()).set("block#", static_cast<int>(aBlockNum));
    aRowCol.push_back(std::move(aRow));
    return true;
  });

  // create the tabular view and show
  TabularView aView(aRowCol);
  IntList widths{20, 20};
  StringList keys{"key", "block#"};
  aView.setKeys(keys).setWidth(widths).show(anOutput);

  return {};
}

//In this function, the database will first get the entity, delete the corresponding attributes, and modify the blocks by blockIO in Storage to make permanent change
StatusResult Database::alertAdd(const std::string& aName, const std::string& aFname, std::ostream& anOutput) {
    if (Entity* theEntity = getEntity(aName)) {//first get the entity
        Attribute theAttribute;
        theAttribute.setName(aName);
        theEntity->addAttribute(theAttribute);//add the attribute to the entity
    }
    //then I need to change the rows, I need to use select row to change the right rows
    
  return {};
}
//In this function, I need to delete the chosen attribute in the attributes in the entity
StatusResult Database::alertDrop(const std::string& aName, const std::string& aFname, std::ostream& anOutput) {
    if (Entity* theEntity = getEntity(aName)) {//first get the entity
        auto attribute = theEntity -> getAttribute(aName);//get the entity from attribute
        theEntity->dropAttribute(*attribute);//drop the attribute in the entity
    }
    //then I need to change the rows
    
    
    DBQuery query;//create a dbquery to select
    RowCollection aRowCol;
    selectRows(query, aRowCol);//select the right rows to delete
    //then I should erase the rows by the storage
//    theEntity->erase(); I need to erase the rows in the entity, by adding erase to the entity class
    return {};
}

}  // namespace ECE141
