//
//  SQLStatement.hpp
//  assignment2_v2_sup
//
//  Created by Maverick on 4/15/22.

#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include <memory>

#include "DBQuery.hpp"
#include "Entity.hpp"
#include "Statement.hpp"

namespace ECE141 {

class Database;

class SQLStatement : public Statement {
 public:
  explicit SQLStatement(Database *aDatabase, Keywords aType = Keywords::unknown_kw)
      : Statement(aType), name(""), database(aDatabase) {}

 protected:
  std::string name;
  Database *database;
};

class CreateTBStatement : public SQLStatement {
 public:
  explicit CreateTBStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::create_kw), entity(name) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;

 protected:
  Entity entity;
};

class DropTBStatement : public SQLStatement {
 public:
  explicit DropTBStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::drop_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class DescribeTBStatement : public SQLStatement {
 public:
  explicit DescribeTBStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::describe_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class ShowTBStatement : public SQLStatement {
 public:
  explicit ShowTBStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::show_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class InsertTBStatement : public SQLStatement {
 public:
  explicit InsertTBStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::insert_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;

 protected:
  StringList fieldList;
  std::vector<StringList> valueList;
};

class SelectStatement : public SQLStatement {
 public:
  explicit SelectStatement(Database *aDatabase, Keywords aKeyword = Keywords::select_kw)
      : SQLStatement(aDatabase, aKeyword) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;

 protected:
  StatusResult parseWhereClause(Tokenizer &aTokenizer);
  StatusResult parseLimitClause(Tokenizer &aTokenizer);
  StatusResult parseOrderbyClause(Tokenizer &aTokenizer);
  StatusResult parseJoin(Tokenizer &aTokenizer);

  DBQuery query;
};

class UpdateStatement : public SelectStatement {
 public:
  explicit UpdateStatement(Database *aDatabase)
      : SelectStatement(aDatabase, Keywords::update_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;

 protected:
  KeyValues map;
};

class DeleteStatement : public SelectStatement {
 public:
  explicit DeleteStatement(Database *aDatabase)
      : SelectStatement(aDatabase, Keywords::delete_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class ShowIndexesStatement : public SQLStatement {
 public:
  explicit ShowIndexesStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::show_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class ShowIndexStatement : public SQLStatement {
 public:
  explicit ShowIndexStatement(Database *aDatabase)
      : SQLStatement(aDatabase, Keywords::show_kw) {}

  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;

 protected:
  std::string fname;
};
//In order to support the alter table... I'll need to create a new class to handle that statement type. We put all the statement about SQL in the SQLStatement, so I create AlterStatement in the SQLStatement.hpp
//then I need to implement recognizes, parse and run in the .cpp file
class AlterAddStatement: public SelectStatement{
public:
 explicit AlterAddStatement(Database *aDatabase)
     : SelectStatement(aDatabase, Keywords::alert_kw) {}

 static bool recognizes(Tokenizer &aTokenizer);
 StatusResult parse(Tokenizer &aTokenizer) override;
 StatusResult run(std::ostream &aStream) override;

protected:
 std::string fname;
    DBQuery query;
};
//here I create alterDropStatement, then I need to implement recognizes, parse and run in the .cpp file
class AlterDropStatement: public SelectStatement{
public:
 explicit AlterDropStatement(Database *aDatabase)
     : SelectStatement(aDatabase, Keywords::alert_kw) {}

 static bool recognizes(Tokenizer &aTokenizer);
 StatusResult parse(Tokenizer &aTokenizer) override;
 StatusResult run(std::ostream &aStream) override;

protected:
 std::string fname;
    DBQuery query;
};

}  // namespace ECE141

#endif /* SQLStatement_hpp */
