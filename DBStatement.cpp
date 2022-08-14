//
//  DBStatement.cpp
//  assignment2_v2_sup
//
//  Created by Maverick on 4/15/22.
//

#include "DBStatement.hpp"
#include "Application.hpp"
#include "TokenSequencer.hpp"

namespace ECE141 {

// create database statement
bool CreateDBStatement::recognizes(Tokenizer &aTokenizer) {
  if (3 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    auto aList = { Keywords::create_kw, Keywords::database_kw };
    return theSeq.nextIs(aList);
  }
  return false;
}

StatusResult CreateDBStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(2).data;
  aTokenizer.next(3);
  return StatusResult{};
}

StatusResult CreateDBStatement::run([[maybe_unused]]std::ostream &aStream) {
  return app->createDatabase(name);
}

// drop database statement
bool DropDBStatement::recognizes(Tokenizer &aTokenizer) {
  if (3 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    auto aList = { Keywords::drop_kw, Keywords::database_kw };
    return theSeq.nextIs(aList);
  }
  return false;
}

StatusResult DropDBStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(2).data;
  aTokenizer.next(3);
  return StatusResult{};
}

StatusResult DropDBStatement::run([[maybe_unused]]std::ostream &aStream) {
  return app->dropDatabase(name);
}

// show database statement
bool ShowDBStatement::recognizes(Tokenizer &aTokenizer) {
  if (2 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    auto aList = { Keywords::show_kw, Keywords::databases_kw };
    return theSeq.nextIs(aList);
  }
  return false;
}

StatusResult ShowDBStatement::parse(Tokenizer &aTokenizer) {
  aTokenizer.next(2);
  return StatusResult{};
}

StatusResult ShowDBStatement::run([[maybe_unused]]std::ostream &aStream) {
  return app->showDatabases();
}

// Use database statement
bool UseDBStatement::recognizes(Tokenizer &aTokenizer) {
  if (2 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    auto aList = { Keywords::use_kw };
    return theSeq.nextIs(aList);
  }
  return false;
}

StatusResult UseDBStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(1).data;
  aTokenizer.next(2);
  return StatusResult{};
}

StatusResult UseDBStatement::run([[maybe_unused]]std::ostream &aStream) {
  return app->useDatabase(name);
}

// dump database statement
bool DumpDBStatement::recognizes(Tokenizer &aTokenizer) {
  if (3 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    return theSeq.nextIs({ Keywords::dump_kw, Keywords::database_kw });
  }
  return false;
}

StatusResult DumpDBStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(2).data;
  aTokenizer.next(3);
  return {};
}

StatusResult DumpDBStatement::run([[maybe_unused]]std::ostream &aStream) {
  return app->dumpDatabase(name);
}

// run script statement
bool RunScriptStatement::recognizes(Tokenizer &aTokenizer) {
  if (3 <= aTokenizer.remaining()) {
    TokenSequencer theSeq(aTokenizer);
    return theSeq.nextIs({ Keywords::run_kw, Keywords::script_kw });
  }
  return false;
}

StatusResult RunScriptStatement::parse(Tokenizer &aTokenizer) {
  name = aTokenizer.peek(2).data;
  aTokenizer.next(3);
  return {};
}

StatusResult RunScriptStatement::run([[maybe_unused]]std::ostream &aStream) {
  return app->runScript(name);
}

}
