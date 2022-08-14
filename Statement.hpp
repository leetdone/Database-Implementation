//
//  Statement.hpp
//  Database
//
//  Created by rick gessner on 3/20/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Statement_hpp
#define Statement_hpp

#include "keywords.hpp"
#include <iostream>

namespace ECE141 {

class Tokenizer;

class Statement {
public:
  Statement(Keywords aStatementType=Keywords::unknown_kw): stmtType(aStatementType) {}
  Statement(const Statement &aCopy)=default;
  virtual ~Statement()=default;

  virtual   StatusResult  parse([[maybe_unused]]Tokenizer &aTokenizer) {return {};}
  Keywords                getType() const {return stmtType;}
  virtual   const char*   getStatementName() const {return "statement";}
  virtual   StatusResult  run([[maybe_unused]]std::ostream &aStream) {return {};}
protected:
  Keywords   stmtType;
};

}

#endif /* Statement_hpp */
