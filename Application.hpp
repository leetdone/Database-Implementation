//
//  AppProcessor.hpp
//  Database1
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include "CmdProcessor.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {

class Database;

class Application : public CmdProcessor {
public:
  
  explicit Application(std::ostream &anOutput);
  virtual ~Application() = default;

  //app api...
  std::string   getVersion() {return "1.0";}
  
  virtual StatusResult  handleInput(std::istream &anInput);
  CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
  Statement*    makeStatement(Tokenizer &aTokenizer, [[maybe_unused]]StatusResult &aResult) override;
  StatusResult  run(Statement *aStmt) override;
  StatusResult  runScript(const std::string& aName);
  
  // function used by dbstatement
  StatusResult  createDatabase(const std::string &aName);
  StatusResult  dumpDatabase(const std::string &aName);
  StatusResult  dropDatabase(const std::string &aName);
  StatusResult  showDatabases() const;
  StatusResult  useDatabase(const std::string &aName);
  bool          dbExists(const std::string &aDBName);
  Database*     getDatabaseInUse() {return database.get();}
protected:
//  Database*     loadDatabase(const std::string &aName);
//  Application&  releaseDatabase();
  
  DBProcessor                 dbProc;
  std::unique_ptr<Database>   database;
};

}

#endif /* Application_hpp */
