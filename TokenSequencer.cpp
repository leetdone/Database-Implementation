//
//  TokenSequencer.cpp
//  assignment2_v2_sup
//
//  Created by Maverick on 4/15/22.
//

#include "TokenSequencer.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "Joins.hpp"
#include "Database.hpp"

namespace ECE141 {

// check next keyword
// we can not only check a single word. we can check a list of word!
TokenSequencer& TokenSequencer::nextIs(const KWList &aList) {
  if (state) {
    std::vector<Keywords> theList{aList};
    int thePos{0};
    for (auto theKW : theList) {
      Token &theToken = tokenizer.peek(thePos++);
      // if find it, just continue the for loop
      // if can't find it, break the for loop
      if (theKW != theToken.keyword) {
        state = false;
        break;
      }
    }
  }
  return *this;
}

// transform the string to integer
TokenSequencer& TokenSequencer::getNumber(int &aValue) {
  const Token &theToken = tokenizer.current();
  aValue = std::stoi(theToken.data);
  return *this;
}

// skip the specic char
TokenSequencer& TokenSequencer::skipPast(char aChar) {
  if (state) {
    tokenizer.next(-1);
    while (tokenizer.next()) {
      auto theToken = tokenizer.current();
      if (theToken.type == TokenType::punctuation) {
        if (theToken.data[0] == aChar) {
          tokenizer.next(); // skip it...
          return *this;
        }
      }
    }
    state = false;
  }
  return *this;
}

// skip a number of word
TokenSequencer& TokenSequencer::skip(size_t aCount) {
  if (state) {
    tokenizer.next(static_cast<int>(aCount)); // actually skip...
  }
  return *this;
}

TokenSequencer& TokenSequencer::clear() {
  state = true;
  return *this;
}

TokenSequencer& TokenSequencer::getIdentifier(std::string &aString) {
  aString = tokenizer.current().data;
  tokenizer.next();
  return *this;
}

TokenSequencer& TokenSequencer::parseAttributes(Entity& anEntity) {
  while (state && tokenizer.more()) {
    Token& theToken = tokenizer.current();
    if (TokenType::identifier == theToken.type) {
      Attribute theAttribute;
      // parse each line of attribute, add them into entity
      if (TokenSequencer::parseAttribute(theAttribute)) {
        if (!anEntity.getAttribute(theAttribute.getName())) {
          anEntity.addAttribute(theAttribute);
          tokenizer.skipIf(',');
        } else {
          state = false;
        }
      }
    } else if (right_paren == theToken.data[0]) {
      break;
    } else state = false;
  }
  return *this;
}

TokenSequencer& TokenSequencer::parseAttribute(Attribute& anAttribute) {
  // set name
  auto theIdentifier = tokenizer.current().data;
  anAttribute.setName(theIdentifier);
  tokenizer.next();
  // set type
  auto theStr = tokenizer.current().data;
  DataTypes theType = Helpers::stringToDataType(theStr);
  if (DataTypes::no_type != theType) {
    anAttribute.setDataType(theType);
    int theSize = 0;
    if (DataTypes::varchar_type == theType && this->skipPast(left_paren).getNumber(theSize).skipPast(right_paren)) {
      anAttribute.setSize(theSize);
    } else this->skip(1);
  } else state = false;
  while ("," != tokenizer.current().data && ")" != tokenizer.current().data) {
    if (Keywords::auto_increment_kw == tokenizer.current().keyword) {
      if (!flag) {
        anAttribute.setAutoIncrement(true);
        flag = true;
        tokenizer.next();
      } else {
        state = false;
        break;
      }
    } else if (Keywords::primary_kw == tokenizer.current().keyword &&
               Keywords::key_kw == tokenizer.peek(1).keyword) {
      anAttribute.setPrimaryKey(true);
      tokenizer.next(2);
    } else if (Keywords::not_kw == tokenizer.current().keyword &&
               Keywords::null_kw == tokenizer.peek(1).keyword) {
      anAttribute.setNullable(false);
      tokenizer.next(2);
    } else if (Keywords::default_kw == tokenizer.current().keyword) {
      auto defaultStr = tokenizer.peek(1).data;
      Value theValue = Helpers::stringToVariant(defaultStr, theType);
      anAttribute.setDefaultValue(theValue);
      tokenizer.next(2);
    }
  }
  return *this;
}

TokenSequencer& TokenSequencer::hasClosedParen() {
  if (tokenizer.skipTo(left_paren)) {
    if (tokenizer.skipTo(right_paren)) {
      // has closed parenthesis
    } else state = false;
  } else state = false;
  return *this;
}

TokenSequencer& TokenSequencer::restart() {
  tokenizer.restart();
  return *this;
}

TokenSequencer& TokenSequencer::parseIdentifierList(StringList &aList) {
  while(state && tokenizer.more()) {
    Token &theToken=tokenizer.current();
    if(TokenType::identifier==tokenizer.current().type) {
      aList.push_back(theToken.data);
      tokenizer.next(); //skip identifier...
      tokenizer.skipIf(comma);
    }
    else if(theToken.type==TokenType::keyword) {
      break; //Auto stop if we see a keyword...
    }
    else if(tokenizer.skipIf(right_paren)){
      break;
    }
    else if(semicolon==theToken.data[0]) {
      break;
    }
    else state = false;
  }
  return *this;
}

TokenSequencer& TokenSequencer::parseValueList(StringList &aList) {
  while(state && tokenizer.more()) {
    Token &theToken=tokenizer.current();
    if(TokenType::identifier==theToken.type || TokenType::number==theToken.type) {
      aList.push_back(theToken.data);
      tokenizer.next(); //skip identifier...
      tokenizer.skipIf(comma);
    }
    else if(tokenizer.skipIf(right_paren)) {
      break;
    }
    else state = false;
  }
  return *this;
}

TokenSequencer& TokenSequencer::parseAssignment(KeyValues& aMap, Entity* anEntity) {
  while (state && tokenizer.more() && Keywords::where_kw != tokenizer.current().keyword) {
    Token& theToken = tokenizer.current();
    if (TokenType::identifier == theToken.type) {
      std::string theName = theToken.data;
      Attribute* theAttribute;
      if ((theAttribute = anEntity->getAttribute(theName))) // can be found in attributelist?
        if ("=" == tokenizer.peek(1).data) {
          DataTypes theType = theAttribute->getType();
          tokenizer.next(2);
          std::string theData = tokenizer.current().data;
          aMap[theName] = Helpers::stringToVariant(theData, theType);
          tokenizer.next();
          tokenizer.skipIf(comma); // skip the comma
          continue;
        }
    }
    state = false;
  }
  return *this;
}

TokenSequencer& TokenSequencer::parseTableField(Entity* lEntity, Entity* rEntity, Operand& anOp) {
  Entity *theEntity;
  std::string lName = lEntity->getName();
  std::string rName = rEntity->getName();
  
  if (lName == tokenizer.current().data) {
    theEntity = lEntity;
  } else if (rName == tokenizer.current().data) {
    theEntity = rEntity;
  } else {
    state = false;
    return *this;
  }
  tokenizer.next();
  
  state = false;
  if (tokenizer.skipIf(dot)) {
    // set the operand
    if (auto* theAttr = theEntity->getAttribute(tokenizer.current().data)) {
      Token &theToken = tokenizer.current();
      anOp.ttype = theToken.type;
      anOp.name = theToken.data;
      anOp.entityId = Helpers::hashString(theEntity->getName().c_str());
      anOp.dtype = theAttr->getType();
      tokenizer.next();
      state = true;
    }
  }
  return *this;
}

TokenSequencer& TokenSequencer::parseTableFieldList(Entity* lEntity, Entity* rEntity, Expression& anExp) {
  if (parseTableField(lEntity, rEntity, anExp.lhs)) {
    if (tokenizer.skipIf(Operators::equal_op)) {
      anExp.op = Operators::equal_op;
      if (parseTableField(lEntity, rEntity, anExp.rhs)) {
        return *this;
      }
    }
  }
  state = false;
  return *this;
}
}
