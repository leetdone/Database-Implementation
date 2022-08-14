//
//  TokenSequencer.hpp
//  assignment2_v2_sup
//
//  Created by Maverick on 4/15/22.

#ifndef TokenSequencer_hpp
#define TokenSequencer_hpp

#include <memory>

#include "BasicTypes.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {

using KWList = std::initializer_list<Keywords>;

class Entity;
class Attribute;
struct Expression;
struct Operand;

class TokenSequencer {
 public:
  explicit TokenSequencer(Tokenizer& aTokenizer)
  : tokenizer(aTokenizer), state{true}, flag{false} {}

  operator bool() { return state; }
  TokenSequencer& clear();
  TokenSequencer& restart();
  TokenSequencer& nextIs(const KWList& aList);
  TokenSequencer& getNumber(int& aValue);
  TokenSequencer& skip(size_t aCount);
  TokenSequencer& skipPast(char aChar);
  TokenSequencer& hasClosedParen();
  TokenSequencer& getIdentifier(std::string& aString);
  TokenSequencer& parseAttributes(Entity& anEntity);
  TokenSequencer& parseAttribute(Attribute& anAttribute);
  TokenSequencer& parseIdentifierList(StringList& aList);
  TokenSequencer& parseValueList(StringList& aList);
  TokenSequencer& parseAssignment(KeyValues& aMap, Entity* anEntity);
  TokenSequencer& parseTableField(Entity* lEntity, Entity* rEntity, Operand& anOp);
  TokenSequencer& parseTableFieldList(Entity* lEntity, Entity* rEntity, Expression& anExp);

 protected:
  Tokenizer& tokenizer;
  bool state;
  bool flag;  // check primary key
};

}  // namespace ECE141

#endif /* TokenSequencer_hpp */
