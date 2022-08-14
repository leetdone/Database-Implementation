//
//  SQLProcessor.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#include <array>
#include "SQLProcessor.hpp"
#include "SQLStatement.hpp"


namespace ECE141 {

SQLProcessor::SQLProcessor(std::ostream &anOutput, Application *anApp)
    : CmdProcessor(anOutput), app(anApp) {
}

SQLProcessor::~SQLProcessor() {}

CmdProcessor *SQLProcessor::recognizes(Tokenizer &aTokenizer) {
  if (CreateTBStatement::recognizes(aTokenizer) ||
      DropTBStatement::recognizes(aTokenizer) ||
      ShowTBStatement::recognizes(aTokenizer) ||
      DescribeTBStatement::recognizes(aTokenizer) ||
      ShowTBStatement::recognizes(aTokenizer) ||
      InsertTBStatement::recognizes(aTokenizer) ||
      SelectStatement::recognizes(aTokenizer) ||
      UpdateStatement::recognizes(aTokenizer) ||
      DeleteStatement::recognizes(aTokenizer) ||
      ShowIndexesStatement::recognizes(aTokenizer) ||
      ShowIndexStatement::recognizes(aTokenizer)) {
    return this;
  }
  return nullptr;
}

using TBStmtFactory = Statement* (SQLProcessor::*)(Database *aDatabase);

Statement* SQLProcessor::createTBStatementFactory(Database *aDatabase) {
  return new CreateTBStatement(aDatabase);
}

Statement* SQLProcessor::dropTBStatementFactory(Database *aDatabase) {
  return new DropTBStatement(aDatabase);
}

Statement* SQLProcessor::showTBStatementFactory(Database *aDatabase) {
  return new ShowTBStatement(aDatabase);
}

Statement* SQLProcessor::describeTBStatementFactory(Database *aDatabase) {
  return new DescribeTBStatement(aDatabase);
}

Statement* SQLProcessor::insertTBStatementFactory(Database *aDatabase) {
  return new InsertTBStatement(aDatabase);
}

Statement* SQLProcessor::selectStatementFactory(Database *aDatabase) {
  return new SelectStatement(aDatabase);
}

Statement* SQLProcessor::updateStatementFactory(Database *aDatabase) {
  return new UpdateStatement(aDatabase);
}

Statement* SQLProcessor::deleteStatementFactory(Database *aDatabase) {
  return new DeleteStatement(aDatabase);
}

Statement* SQLProcessor::showIndexesStatementFactory(Database *aDatabase) {
  return new ShowIndexesStatement(aDatabase);
}

Statement* SQLProcessor::showIndexStatementFactory(Database *aDatabase) {
  return new ShowIndexStatement(aDatabase);
}
Statement* SQLProcessor::alertAddStatementFactory(Database *aDatabase) {
    return new AlterAddStatement(aDatabase);
}
Statement* SQLProcessor::alertDropStatementFactory(Database *aDatabase) {
    return new AlterDropStatement(aDatabase);
}

using ShowKeywords = std::array<Keywords, 2>;

Statement *SQLProcessor::makeStatement(Tokenizer &aTokenizer, [[maybe_unused]] StatusResult &aResult) {
  static std::map<Keywords, TBStmtFactory> factories = {
      {Keywords::create_kw,   &SQLProcessor::createTBStatementFactory},
      {Keywords::drop_kw,     &SQLProcessor::dropTBStatementFactory},
      {Keywords::describe_kw, &SQLProcessor::describeTBStatementFactory},
      {Keywords::insert_kw,   &SQLProcessor::insertTBStatementFactory},
      {Keywords::select_kw,   &SQLProcessor::selectStatementFactory},
      {Keywords::update_kw,   &SQLProcessor::updateStatementFactory},
      {Keywords::delete_kw,   &SQLProcessor::deleteStatementFactory},
      {Keywords::alert_kw, &SQLProcessor::alertAddStatementFactory},
  };

  static std::map<ShowKeywords, TBStmtFactory> showFactories = {
      {{Keywords::show_kw, Keywords::tables_kw}, &SQLProcessor::showTBStatementFactory},
      {{Keywords::show_kw, Keywords::indexes_kw}, &SQLProcessor::showIndexesStatementFactory},
      {{Keywords::show_kw, Keywords::index_kw}, &SQLProcessor::showIndexStatementFactory},
  };

  if (aTokenizer.size()) {
    Token &theToken = aTokenizer.current();
    ShowKeywords theKeywords = {aTokenizer.current().keyword, aTokenizer.peek().keyword};
    
    if (factories.count(theToken.keyword)) {
      if (Statement *theStatement = (this->*(factories[theToken.keyword]))(app->getDatabaseInUse())) {
        if (theStatement->parse(aTokenizer)) {
          return theStatement;
        }
      }
    } else if (showFactories.count(theKeywords)) {
      if (Statement *theStatement = (this->*(showFactories[theKeywords]))(app->getDatabaseInUse())) {
        if (theStatement->parse(aTokenizer)) {
          return theStatement;
        }
      }
    }
  }
  return nullptr;
}

StatusResult SQLProcessor::run(Statement *aStmt) {
  return aStmt->run(output);
}

}  // namespace ECE141
