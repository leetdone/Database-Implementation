//
//  DBProcessor.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <map>
#include <fstream>
#include "Config.hpp"
#include "DBProcessor.hpp"
#include "Database.hpp"
#include "Application.hpp"
#include "Helpers.hpp"
#include "DBStatement.hpp"
#include "Storage.hpp"
#include "SQLProcessor.hpp"

namespace ECE141 {

DBProcessor::DBProcessor(std::ostream &anOutput, Application* anApp)
: CmdProcessor(anOutput), app(anApp) {
  sqlProc = new SQLProcessor(anOutput, anApp);
}

DBProcessor::~DBProcessor() {
  // TODO: whether need to delete sqlProc...
}

//CmdProcessor interface ...
CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
  if(CreateDBStatement::recognizes(aTokenizer) ||
     DropDBStatement::recognizes(aTokenizer) ||
     ShowDBStatement::recognizes(aTokenizer) ||
     UseDBStatement::recognizes(aTokenizer) ||
     DumpDBStatement::recognizes(aTokenizer) ||
     RunScriptStatement::recognizes(aTokenizer)) {
    return this;
  }
  return sqlProc->recognizes(aTokenizer);
}

// factory pattern
using DBStmtFactory = Statement* (*)(Application* anApp);

Statement* createDBStatementFactory(Application *anApp) {
  return new CreateDBStatement(anApp);
}

Statement* dropDBStatementFactory(Application *anApp) {
  return new DropDBStatement(anApp);
}

Statement* showDBStatementFactory(Application *anApp) {
  return new ShowDBStatement(anApp);
}

Statement* useDBStatementFactory(Application *anApp) {
  return new UseDBStatement(anApp);
}

Statement* dumpDBStatementFactory(Application *anApp) {
  return new DumpDBStatement(anApp);
}

Statement* runScriptStatementFactory(Application *anApp) {
  return new RunScriptStatement(anApp);
}

Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer, [[maybe_unused]]StatusResult &aResult) {
  static std::map<Keywords, DBStmtFactory> factories = {
    {Keywords::create_kw, createDBStatementFactory},
    {Keywords::drop_kw,   dropDBStatementFactory},
    {Keywords::show_kw,   showDBStatementFactory},
    {Keywords::use_kw,    useDBStatementFactory},
    {Keywords::dump_kw,   dumpDBStatementFactory},
    {Keywords::run_kw,    runScriptStatementFactory},
  };
  
  if (aTokenizer.size()) {
    Token &theToken = aTokenizer.current();
    if(factories.count(theToken.keyword)) {
      if(Statement *theStatement = factories[theToken.keyword](app)) {
        if(theStatement->parse(aTokenizer)) {
          return theStatement;
        }
      }
    }
  }
  return nullptr;
}

StatusResult DBProcessor::run(Statement *aStmt) {
  return aStmt->run(output);
}

}
