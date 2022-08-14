//
//  SQLStatement.cpp
//  assignment2_v2_sup
//
//  Created by Maverick on 4/15/22.

#include "SQLStatement.hpp"

#include "Database.hpp"
#include "Filters.hpp"
#include "Helpers.hpp"
#include "Joins.hpp"
#include "TokenSequencer.hpp"

namespace ECE141 {

// create table statement
bool CreateTBStatement::recognizes(Tokenizer &aTokenizer) {
  const int kMinLength = 8;  // smallest # of tokens in create statement
  if (kMinLength <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    auto aList = {Keywords::create_kw, Keywords::table_kw};
    if (theSeq.nextIs(aList).hasClosedParen().restart()) {
      return true;
    }
  }
  return false;
}

StatusResult CreateTBStatement::parse(Tokenizer &aTokenizer) {
  StatusResult theResult{syntaxError};
  TokenSequencer theSeq(aTokenizer);
  if (theSeq.skip(2).getIdentifier(name)) {
    entity.setName(name);
    if (theSeq.skipPast(left_paren).parseAttributes(entity).skipPast(right_paren)) {
      theSeq.skipPast(';');
      theResult.error = Errors::noError;
    }
  }
  return theResult;
}

StatusResult CreateTBStatement::run(std::ostream &aStream) {
  if (database) return database->createTable(entity, aStream);
  return {noDatabaseSpecified};
}

// drop table statement
bool DropTBStatement::recognizes(Tokenizer &aTokenizer) {
  if (3 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    return theSeq.nextIs({Keywords::drop_kw, Keywords::table_kw});
  }
  return false;
}

StatusResult DropTBStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(2).data;
  aTokenizer.next(3);
  return {};
}

StatusResult DropTBStatement::run(std::ostream &aStream) {
  if (database) return database->dropTable(name, aStream);
  return {noDatabaseSpecified};
}

// describe table statement
bool DescribeTBStatement::recognizes(Tokenizer &aTokenizer) {
  if (2 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    return theSeq.nextIs({Keywords::describe_kw});
  }
  return false;
}

StatusResult DescribeTBStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(1).data;
  aTokenizer.next(2);
  return {};
}

StatusResult DescribeTBStatement::run(std::ostream &aStream) {
  if (database) return database->describeTable(name, aStream);
  return {noDatabaseSpecified};
}

// show table statement
bool ShowTBStatement::recognizes(Tokenizer &aTokenizer) {
  if (2 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    return theSeq.nextIs({Keywords::show_kw, Keywords::tables_kw});
  }
  return false;
}

StatusResult ShowTBStatement::parse(Tokenizer &aTokenizer) {
  aTokenizer.next(2);
  return {};
}

StatusResult ShowTBStatement::run(std::ostream &aStream) {
  if (database) return database->showTables(aStream);
  return {noDatabaseSpecified};
}

// insert table statement
bool InsertTBStatement::recognizes(Tokenizer &aTokenizer) {
  bool flag = false;
  const int kMinLength = 10;  // smallest # of tokens in insert statement
  TokenSequencer theSeq(aTokenizer);
  if (kMinLength <= aTokenizer.remaining()) {
    auto aList = {Keywords::insert_kw, Keywords::into_kw};
    if (theSeq.nextIs(aList).hasClosedParen()) {
      auto anoList = {Keywords::values_kw};
      if (theSeq.skipPast(')').nextIs(anoList).hasClosedParen().restart())
        flag = true;
    }
  }
  theSeq.restart();
  return flag;
}

StatusResult InsertTBStatement::parse(Tokenizer &aTokenizer) {
  TokenSequencer theSeq(aTokenizer);
  if (theSeq.skip(2).getIdentifier(name).skipPast(left_paren).parseIdentifierList(fieldList))
    while (aTokenizer.more() && semicolon != aTokenizer.current().data[0]) {
      StringList aStrList;
      if (theSeq.skipPast(left_paren).parseValueList(aStrList))
        valueList.push_back(aStrList);
      else
        return {syntaxError};
    }
  else
    return {syntaxError};
  return {};
}

StatusResult InsertTBStatement::run(std::ostream &aStream) {
  StatusResult theResult;
  ValueList theValueList;
  Entity *theEntity;

  if (!database) return {noDatabaseSpecified};
  if ((theEntity = database->getEntity(name))) {
    if ((theResult = theEntity->validateFieldList(fieldList))) {
      size_t temp = 0;
      for (auto &lst : valueList) {
        if (theEntity->validateValueList(fieldList, lst, theValueList)) temp++;
      }
      if (temp == valueList.size())
        database->insertRows(theValueList, name, aStream);
      else
        theResult.error = unexpectedValue;
    }
  }
  return theResult;
}

// select table statement
bool SelectStatement::recognizes(Tokenizer &aTokenizer) {
  TokenSequencer theSeq(aTokenizer);
  bool flag = false;
  const int kMinLength = 4;  // smallest # of tokens in select statement
  if (kMinLength <= aTokenizer.remaining()) {
    if (theSeq.nextIs({Keywords::select_kw}))
      if (aTokenizer.skipTo(Keywords::from_kw))
        flag = true;
  }
  theSeq.restart();
  return flag;
}

StatusResult SelectStatement::parseWhereClause(Tokenizer &aTokenizer) {
  aTokenizer.skipIf(Keywords::where_kw);
  Filters &theFilter = query.getFilter();  // use filter to parse where clause
  return theFilter.parse(aTokenizer, *database->getEntity(name));
}

StatusResult SelectStatement::parseLimitClause(Tokenizer &aTokenizer) {
  std::string theData = aTokenizer.peek(1).data;
  size_t theLimit = std::stoul(theData);
  query.setLimit(theLimit);
  aTokenizer.next(2);
  return {};
}

StatusResult SelectStatement::parseOrderbyClause(Tokenizer &aTokenizer) {
  std::string theData = aTokenizer.peek(2).data;
  query.setOrderBy(theData);
  aTokenizer.next(3);
  return {};
}

StatusResult SelectStatement::parseJoin(Tokenizer &aTokenizer) {
  TokenSequencer aSeq(aTokenizer);
  const Token &theToken = aTokenizer.current();
  StatusResult theResult{joinTypeExpected};

  Keywords theJoinType = theToken.keyword;
  aTokenizer.next(1);
  if (aTokenizer.skipIf(Keywords::join_kw)) {
    std::string theName;
    if (aSeq.getIdentifier(theName)) {
      theResult.error = keywordExpected;
      if (aTokenizer.skipIf(Keywords::on_kw)) {
        theResult.error = unknownTable;
        if (auto *lEntity = database->getEntity(name)) {
          if (auto *rEntity = database->getEntity(theName)) {
            theResult.error = syntaxError;
            Expression theExpression;
            if (aSeq.parseTableFieldList(lEntity, rEntity, theExpression)) {
              query.addJoin(theName, theJoinType, theExpression);
              theResult.error = noError;
            }
          }
        }
      }
    }
  }
  return theResult;
}

StatusResult SelectStatement::parse(Tokenizer &aTokenizer) {
  // used by checking the join type
  static Keywords gJoinTypes[] = {Keywords::cross_kw, Keywords::full_kw, Keywords::inner_kw,
                                  Keywords::left_kw, Keywords::right_kw};

  StatusResult theResult;
  TokenSequencer aSeq(aTokenizer);
  StringList aList;
  // parse identifier list
  aTokenizer.skipIf(Keywords::select_kw);
  if ("*" == aTokenizer.current().data) {
    query.setFieldList({"*"});
  } else if (aSeq.parseIdentifierList(aList)) {
    query.setFieldList(aList);
  }
  // parse table name
  aTokenizer.skipTo(Keywords::from_kw);
  name = aTokenizer.peek(1).data;
  query.setTableName(name);
  aTokenizer.next(2);
  // parse where, limit, orderby clause
  while (theResult && aTokenizer.more() && semicolon != aTokenizer.current().data[0]) {
    Keywords theKeyword = aTokenizer.current().keyword;
    if (Keywords::where_kw == theKeyword) {
      theResult = parseWhereClause(aTokenizer);
    } else if (Keywords::limit_kw == theKeyword) {
      theResult = parseLimitClause(aTokenizer);
    } else if (Keywords::order_kw == theKeyword) {
      theResult = parseOrderbyClause(aTokenizer);
    } else if (in_array<Keywords>(gJoinTypes, theKeyword)) {
      theResult = parseJoin(aTokenizer);
    }
  }
  return theResult;
}

StatusResult SelectStatement::run(std::ostream &aStream) {
  if (database) return database->showQuery(query, aStream);
  return StatusResult{noDatabaseSpecified};
}

// * update statement
// * UPDATE Users SET zipcode = 92127 WHERE zipcode>92100;
bool UpdateStatement::recognizes(Tokenizer &aTokenizer) {
  bool flag = false;
  const int kMinLength = 10;  // smallest # of tokens in insert statement
  TokenSequencer theSeq(aTokenizer);
  if (kMinLength <= aTokenizer.remaining())
    if (theSeq.nextIs({Keywords::update_kw}).skip(2).nextIs({Keywords::set_kw}))
      if (aTokenizer.skipTo(Keywords::where_kw))
        flag = true;
  theSeq.restart();
  return flag;
}

StatusResult UpdateStatement::parse(Tokenizer &aTokenizer) {
  StatusResult theResult;
  TokenSequencer theSeq(aTokenizer);
  theSeq.skip(1);
  name = aTokenizer.current().data;
  query.setTableName(name);
  theSeq.skip(2);
  Entity *theEntity = database->getEntity(name);
  theSeq.parseAssignment(map, theEntity);
  theResult = parseWhereClause(aTokenizer);
  return theResult;
}

StatusResult UpdateStatement::run(std::ostream &aStream) {
  if (database) return database->updateRows(query, map, aStream);
  return {noDatabaseSpecified};
}

// * delete statement
// * DELETE from Users where zipcode>92124;
bool DeleteStatement::recognizes(Tokenizer &aTokenizer) {
  bool flag = false;
  const int kMinLength = 7;  // smallest # of tokens in insert statement
  TokenSequencer theSeq(aTokenizer);
  if (kMinLength <= aTokenizer.remaining())
    if (theSeq.nextIs({Keywords::delete_kw, Keywords::from_kw}))
      if (aTokenizer.skipTo(Keywords::where_kw))
        flag = true;
  theSeq.restart();
  return flag;
}

StatusResult DeleteStatement::parse(Tokenizer &aTokenizer) {
  StatusResult theResult;
  aTokenizer.next(2);
  name = aTokenizer.current().data;
  query.setTableName(name);
  aTokenizer.next(1);
  theResult = parseWhereClause(aTokenizer);
  return theResult;
}

StatusResult DeleteStatement::run(std::ostream &aStream) {
  if (database) return database->deleteRows(query, aStream);
  return {noDatabaseSpecified};
}

// * show indexes statement
// * show indexes;
bool ShowIndexesStatement::recognizes(Tokenizer &aTokenizer) {
  const int kMinLength = 2;
  if (kMinLength <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    return theSeq.nextIs({Keywords::show_kw, Keywords::indexes_kw});
  }
  return false;
}

StatusResult ShowIndexesStatement::parse(Tokenizer &aTokenizer) {
  aTokenizer.next(2);
  return {};
}

StatusResult ShowIndexesStatement::run(std::ostream &aStream) {
  if (database) return database->showIndexes(aStream);
  return {noDatabaseSpecified};
}

//ALTER TABLE Books ADD  pub_year int;  -- adding a new column
//According to the command above, whether I can recognize the command depends on the first token "ALTER",
//I need to add these command to SQLProcessor as well
bool AlterAddStatement::recognizes(Tokenizer &aTokenizer) {
  const int kMinLength = 5; //THE command will be 5, which is the minimum nuber of command length
    bool flag = false;
    TokenSequencer theSeq(aTokenizer);
    if (kMinLength <= aTokenizer.remaining())
      if (theSeq.nextIs({Keywords::alert_kw}))//recognize the first token should be "alert"
        if (aTokenizer.skipTo(Keywords::add_kw))//recognize the forth token should be "add"
          flag = true;
    theSeq.restart();
    return flag;
}

StatusResult AlterAddStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.next(2);
    name = aTokenizer.current().data;// get the table name
    aTokenizer.next(2);
    fname = aTokenizer.current().data; // get field name
    return {};
}
//run the alertAdd in the database
StatusResult AlterAddStatement::run(std::ostream &aStream) {
  if (database) return database->alertAdd(name, fname,aStream);//after making the statement, I will send the statement to database, so I need to implement alertAdd in database class. I will add the statement to the SQLProcessor as well
    //run the database command based on table name and field name
  return {noDatabaseSpecified};
}

//ALTER TABLE Books DROP subtitle;  -- dropping an existing column,
//I need to add these command to SQLProcessor as well
bool AlterDropStatement::recognizes(Tokenizer &aTokenizer) {
    const int kMinLength = 5; //THE command will be 5, which is the minimum nuber of command length
      bool flag = false;
      TokenSequencer theSeq(aTokenizer);
      if (kMinLength <= aTokenizer.remaining())
        if (theSeq.nextIs({Keywords::alert_kw}))//recognize the first token should be "alert"
          if (aTokenizer.skipTo(Keywords::drop_kw))//recognize the forth token should be "add"
            flag = true;
      theSeq.restart();
      return flag;
}

StatusResult AlterDropStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.next(2);
    name = aTokenizer.current().data;// get the table name
    aTokenizer.next(2);
    fname = aTokenizer.current().data; // get field name
    return {};
}
//run the alertAdd in the database
StatusResult AlterDropStatement::run(std::ostream &aStream) {
  if (database) return database->alertDrop(name, fname, aStream);//after making the statement, I will send the statement to database, so I need to implement alertDrop in database class, I will add the statement to the SQLProcessor as well
    //run the database command based on table name and field name
  return {noDatabaseSpecified};
}

// * show index statement
// * SHOW index id FROM Users;
bool ShowIndexStatement::recognizes(Tokenizer &aTokenizer) {
  const int kMinLength = 5;
  if (kMinLength <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    if (theSeq.nextIs({Keywords::show_kw, Keywords::index_kw})) {
      if (aTokenizer.skipTo(Keywords::from_kw)) {
        theSeq.restart();
        return true;
      }
    }
  }
  return false;
}

StatusResult ShowIndexStatement::parse(Tokenizer &aTokenizer) {
  aTokenizer.next(2);
  fname = aTokenizer.current().data; // get field name
  aTokenizer.next(2);
  name = aTokenizer.current().data;  // get table name;
  aTokenizer.next();
  return {};
}

StatusResult ShowIndexStatement::run(std::ostream &aStream) {
  if (database) return database->showIndexFrom(name, fname, aStream);
  return {noDatabaseSpecified};
}

}  // namespace ECE141
