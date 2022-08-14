//
//  CommandProcessor.cpp
//
//  Created by rick gessner on 3/30/20
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Application.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "Config.hpp"
#include "Database.hpp"
#include "FolderReader.hpp"
#include "TabularView.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {

size_t Config::cacheSize[] = {0, 0, 0};

Application::Application(std::ostream &anOutput)
    : CmdProcessor(anOutput), dbProc(anOutput, this), database(nullptr) {
}

// USE: -----------------------------------------------------

bool isKnown(Keywords aKeyword) {
  static Keywords theKnown[] =
      {Keywords::quit_kw, Keywords::version_kw, Keywords::help_kw};
  auto theIt = std::find(std::begin(theKnown),
                         std::end(theKnown), aKeyword);
  return theIt != std::end(theKnown);
}

/*
 * edited by Hongbo
 * if it is not application command
 * give it to the dbprocessor to test whether it is a database command...
 */
CmdProcessor *Application::recognizes(Tokenizer &aTokenizer) {
  if (isKnown(aTokenizer.current().keyword)) {
    return this;
  } else if (auto aProc = dbProc.recognizes(aTokenizer)) {
    return aProc;
  }
  return nullptr;
}

StatusResult Application::run(Statement *aStatement) {
  switch (aStatement->getType()) {
    case Keywords::quit_kw:
      output << "DB::141 is shutting down\n";
      return {userTerminated};
      break;
    case Keywords::version_kw:
      output << "Version " << getVersion() << "\n";
      break;
    case Keywords::help_kw:
      output << "Help system available\n";
      break;
    default:
      break;
  }
  return {noError};
}

StatusResult Application::runScript(const std::string &aName) {
  // ! This folder path may need to be changed each project
  std::string folderPath = "/Users/qianhongbo/Documents/Spring22/ECE141B/assignment/assignment9-group6/";
  std::string thePath = folderPath + aName + ".txt";
  auto theScripts = std::fstream(thePath, std::ios::in);
  ECE141::StatusResult theResult;
  std::string theCommand;

  while (theResult && theScripts) {
    std::getline(theScripts, theCommand);
    std::stringstream theStream(theCommand);
    std::cout << theCommand << "\n";
    theResult = handleInput(theStream);
    std::cout << "\n";
  }
  return theResult;
}

// USE: retrieve a statement based on given text input...
Statement *Application::makeStatement(Tokenizer &aTokenizer, [[maybe_unused]] StatusResult &aResult) {
  Token theToken = aTokenizer.current();
  if (isKnown(theToken.keyword)) {
    aTokenizer.next();  // skip ahead...
    return new Statement(theToken.keyword);
  }

  return nullptr;
}

// build a tokenizer, tokenize input, ask processors to handle...
StatusResult Application::handleInput(std::istream &anInput) {
  Tokenizer theTokenizer(anInput);
  StatusResult theResult = theTokenizer.tokenize();

  while (theResult && theTokenizer.more()) {
    Config::getTimer().reset();
    if (auto *theProc = recognizes(theTokenizer)) {
      if (auto *theCmd = theProc->makeStatement(theTokenizer, theResult)) {
        theResult = theProc->run(theCmd);
        if (theResult) theTokenizer.skipIf(';');
        delete theCmd;
      } else
        theResult = syntaxError;
    } else
      theResult = unknownCommand;
  }
  return theResult;
}

bool Application::dbExists(const std::string &aDBName) {
  std::string path = Config::getDBPath(aDBName);
  return std::filesystem::exists(path);
}

StatusResult Application::createDatabase(const std::string &aName) {
  StatusResult theResult;
  // if the file has existed, output the databaseExists error
  if (!dbExists(aName)) {
    //    CreateDB createMode;
    Database theDatabase(aName, CreateDB{});
    // if the file didn't create successfully,
    // output the databaseCreationError error
    if (dbExists(aName)) {
      ResultView theResultView(1);
      theResultView.show(output);
    } else {
      theResult = {databaseCreationError};
    }
  } else {
    theResult = {databaseExists};
  }
  return theResult;
}

StatusResult Application::showDatabases() const {
  RowCollection aRowCol;
  FolderReader aReader(Config::getStoragePath());
  aReader.each(Config::getDBExtension(), [&](const std::string &aName) {
    std::unique_ptr<Row> aRow = std::make_unique<Row>();
    aRow->set("Database", aName);
    aRowCol.push_back(std::move(aRow));
    return true;
  });
  TabularView theView(aRowCol);
  IntList widths{20};
  StringList keys{"Database"};
  theView.setKeys(std::move(keys)).setWidth(std::move(widths)).show(output);
  return {};
}

// USE: call this to perform the dropping of a database (remove the file)...
StatusResult Application::dropDatabase(const std::string &aName) {
  StatusResult theResult;
  if (dbExists(aName)) {
    std::string path = Config::getDBPath(aName);
    std::filesystem::remove(path);
    ResultView theResultView(0);
    theResultView.show(output);
  } else {
    theResult.error = unknownDatabase;
  }
  return theResult;
}

// USE: DB dump all storage blocks
StatusResult Application::dumpDatabase(const std::string &aName) {
  StatusResult theResult;
  if (database) {
    theResult = database->dump(output);
  } else if (dbExists(aName)) {
    OpenDB openMode;
    Database theDatabase(aName, openMode);
    theResult = theDatabase.dump(output);
  } else {
    theResult.error = unknownDatabase;
  }
  return theResult;
}

// USE: call DB object to be loaded into memory...
StatusResult Application::useDatabase(const std::string &aName) {
  StatusResult theResult{unknownDatabase};
  if (dbExists(aName)) {
    // open the database
    database = std::make_unique<Database>(aName, OpenDB{});
    if ((theResult = database->loadEntityIndex())) {
      if ((theResult = database->loadIndexIndex())) {
        if ((theResult = database->loadAvailableBlock())) {
          output << "Database changed\n";
          theResult.error = noError;
        }
      }
    }
  }
  return theResult;
}

}  // namespace ECE141
