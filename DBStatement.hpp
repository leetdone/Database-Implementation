//
//  DBStatement.hpp
//  assignment2_sup
//
//  Created by Maverick on 4/9/22.
//

#ifndef DBStatement_hpp
#define DBStatement_hpp

#include <string>
#include "Statement.hpp"


namespace ECE141 {

class Application;

class DBStatement: public Statement {
public:
  explicit DBStatement(Application* anApp, Keywords aStatementType = Keywords::unknown_kw)
  : Statement(aStatementType), app(anApp), name("") {}
protected:
  Application*  app;
  std::string   name;
};

class CreateDBStatement: public DBStatement {
public:
  explicit CreateDBStatement(Application* anApp = nullptr)
  : DBStatement(anApp, Keywords::create_kw) {}
  
  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class DropDBStatement: public DBStatement {
public:
  explicit DropDBStatement(Application* anApp = nullptr)
  : DBStatement(anApp, Keywords::drop_kw) {}
  
  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class ShowDBStatement: public DBStatement {
public:
  explicit ShowDBStatement(Application* anApp = nullptr)
  : DBStatement(anApp, Keywords::show_kw) {}
  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class UseDBStatement: public DBStatement {
public:
  explicit UseDBStatement(Application* anApp = nullptr)
  : DBStatement(anApp, Keywords::use_kw) {}
  
  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class DumpDBStatement: public DBStatement {
public:
  explicit DumpDBStatement(Application* anApp = nullptr)
  : DBStatement(anApp, Keywords::dump_kw) {}
  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

class RunScriptStatement: public DBStatement {
public:
  explicit RunScriptStatement(Application* anApp = nullptr)
  : DBStatement(anApp, Keywords::run_kw) {}
  static bool recognizes(Tokenizer &aTokenizer);
  StatusResult parse(Tokenizer &aTokenizer) override;
  StatusResult run(std::ostream &aStream) override;
};

}

#endif /* DBStatement_hpp */
