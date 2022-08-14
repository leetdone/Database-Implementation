//
//  Tokenizer.hpp
//  Database
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <iostream>
#include <vector>
#include <cstring>
#include "keywords.hpp"
#include "Scanner.hpp"

namespace ECE141 {

using parseCallback = bool(char aChar);

enum class TokenType {
  function, identifier, keyword, number, operators, timedate, punctuation, string, unknown
};

//-----------------

struct Token {
  Token& operator=(const Token &aCopy) = default;
  
  TokenType   type;
  Keywords    keyword;
  Operators   op;
  std::string data;
};

//-----------------

class Tokenizer : public Scanner {
public:
  explicit Tokenizer(std::istream &anInputStream);
  
  StatusResult  tokenize();
  Token&        tokenAt(size_t anOffset);
  
  Token&        current();
  bool          more() {return index<size();}
  bool          next(int anOffset=1);
  Token&        peek(int anOffset=1);
  void          restart() {index=0;}
  size_t        size() {return tokens.size();}
  size_t        remaining() {return index<size() ? size()-index :0;}
  
  //these might consume a token...
  bool          skipTo(Keywords aKeyword);
  bool          skipTo(TokenType aTokenType);
  bool          skipTo(char aChar);
  bool          skipIf(Keywords aKeyword);
  bool          skipIf(Operators anOperator);
  bool          skipIf(TokenType aTokenType);
  bool          skipIf(char aChar);
  
  void          dump(); //utility
  
protected:
  
  std::vector<Token>    tokens;
  size_t                index;
};

}

#endif /* Tokenizer_hpp */

