//
//  Filters.cpp
//  Datatabase5
//
//  Created by rick gessner on 3/5/21.
//  Copyright © 2021 rick gessner. All rights reserved.


#include <string>
#include <limits>
#include "Filters.hpp"
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Compare.hpp"

namespace ECE141 {

using Comparitor = bool (*)(Value &aLHS, Value &aRHS);

bool equals(Value &aLHS, Value &aRHS) {
  bool theResult=false;
  
  std::visit([&](auto const &aLeft) {
    std::visit([&](auto const &aRight) {
      theResult=isEqual(aLeft,aRight);
    },aRHS);
  },aLHS);
  return theResult;
}

bool notEquals(Value &aLHS, Value &aRHS) {
  bool theResult=false;
  
  std::visit([&](auto const &aLeft) {
    std::visit([&](auto const &aRight) {
      theResult=!isEqual(aLeft,aRight);
    },aRHS);
  },aLHS);
  return theResult;
}

bool lessThan(Value &aLHS, Value &aRHS) {
  bool theResult=false;
  
  std::visit([&](auto const &aLeft) {
    std::visit([&](auto const &aRight) {
      theResult=isLess(aLeft,aRight);
    },aRHS);
  },aLHS);
  return theResult;
}

bool greaterThan(Value &aLHS, Value &aRHS) {
  bool theResult = false;
  
  std::visit([&](auto const &aLeft) {
    std::visit([&](auto const &aRight) {
      theResult = isGreater(aLeft,aRight);
    },aRHS);
  },aLHS);
  return theResult;
}

bool lessThanEqual(Value &aLHS, Value &aRHS) {
  bool theResult=false;
  
  std::visit([&](auto const &aLeft) {
    std::visit([&](auto const &aRight) {
      theResult = isLess(aLeft,aRight) || isEqual(aLeft, aRight);
    }, aRHS);
  }, aLHS);
  return theResult;
}

bool greaterThanEqual(Value &aLHS, Value &aRHS) {
  bool theResult=false;
  
  std::visit([&](auto const &aLeft) {
    std::visit([&](auto const &aRight) {
      theResult = isGreater(aLeft,aRight) || isEqual(aLeft, aRight);
    },aRHS);
  },aLHS);
  return theResult;
}

static std::map<Operators, Comparitor> comparitors {
  {Operators::equal_op, equals},
  {Operators::notequal_op, notEquals},
  {Operators::lt_op, lessThan},
  {Operators::gt_op, greaterThan},
  {Operators::lte_op, lessThanEqual},
  {Operators::gte_op, greaterThanEqual},
};

bool Expression::operator()(KeyValues &aList) {
  Value theLHS{lhs.value};
  Value theRHS{rhs.value};
  
  if(TokenType::identifier==lhs.ttype) {
    theLHS=aList[lhs.name]; //get row value
  }
  
  if(TokenType::identifier==rhs.ttype) {
    theRHS=aList[rhs.name]; //get row value
  }
  
  return comparitors.count(op)
  ? comparitors[op](theLHS, theRHS) : false;
}

//--------------------------------------------------------------

Filters::Filters()  {}

Filters::~Filters() {
  //no need to delete expressions, they're unique_ptrs!
}

Filters& Filters::add(Expression *anExpression) {
  expressions.push_back(std::unique_ptr<Expression>(anExpression));
  return *this;
}

//compare expressions to row; return true if matches
bool Filters::matches(KeyValues &aList) const {
  std::vector<bool> theVector;
  bool skip = false;
  for(auto &theExpr : expressions) {
    if (skip) continue;
    bool theResult = (*theExpr)(aList);
    if (Logical::and_op == theExpr->logic) {
      if (theResult)
        continue;
      else
        return false;
    } else if (Logical::or_op == theExpr->logic) {
      if (theResult) {
        theVector.push_back(theResult);
        skip = true;
      }
      else
        continue;
    } else
      theVector.push_back(theResult);
  }
  bool res = true;
  size_t theSize = theVector.size();
  for (size_t i = 0; i < theSize; i++) {
    res &= theVector[i];
  }
  return res;
}

//where operand is field, number, string...
StatusResult parseOperand(Tokenizer &aTokenizer, Entity &anEntity, Operand &anOperand) {
  StatusResult theResult;
  Token &theToken = aTokenizer.current();
  if(TokenType::identifier==theToken.type) {
    if(auto *theAttr=anEntity.getAttribute(theToken.data)) {
      anOperand.ttype=theToken.type;
      anOperand.name=theToken.data; //hang on to name...
      anOperand.entityId=Helpers::hashString(anEntity.getName().c_str());
      anOperand.dtype=theAttr->getType();
    }
    else {
      anOperand.ttype=TokenType::string;
      anOperand.dtype=DataTypes::varchar_type;
      anOperand.value=theToken.data;
    }
  }
  else if(TokenType::number==theToken.type) {
    anOperand.ttype=TokenType::number;
    anOperand.dtype=DataTypes::int_type;
    if (theToken.data.find('.')!=std::string::npos) {
      anOperand.dtype=DataTypes::float_type;
      anOperand.value=std::stof(theToken.data);
    }
    else anOperand.value=std::stoi(theToken.data);
  }
  else theResult.error=syntaxError;
  if(theResult) aTokenizer.next();
  return theResult;
}

bool validateHelper(const Operand &op1, const Operand &op2, const Entity& anEntity) {
  Attribute* theAttribute = anEntity.getAttribute(op1.name);
  DataTypes theType = theAttribute->getType();
  if (theType == op2.dtype)  // check the value type of aRHS
    return true;
  return false;
}

//STUDENT: Add validation here...
bool validateOperands(const Operand &aLHS, const Operand &aRHS, const Entity &anEntity) {
  if(TokenType::identifier==aLHS.ttype) { //most common case...
    return validateHelper(aLHS, aRHS, anEntity);
  }
  else if(TokenType::identifier==aRHS.ttype) {
    return validateHelper(aRHS, aLHS, anEntity);
  }
  return false;
}

//STUDENT: This starting point code may need adaptation...
StatusResult Filters::parse(Tokenizer &aTokenizer, Entity &anEntity) {
  StatusResult  theResult;
  
  while(theResult && (2<aTokenizer.remaining())) {
    Operand theLHS,theRHS;
    bool notFlag = false;
    if(Keywords::not_kw == aTokenizer.current().keyword) {
      notFlag = true;
      aTokenizer.next();
    }
    Token &theToken=aTokenizer.current();
    if(theToken.type!=TokenType::identifier) return theResult;
    if((theResult=parseOperand(aTokenizer, anEntity, theLHS))) {
      const Token &theToken=aTokenizer.current();
      if(theToken.type==TokenType::operators) {
        const Token& nextToken = aTokenizer.peek();
        std::string tmp = theToken.data;
        int anOffset = 1;
        if (nextToken.type == TokenType::operators) {
          tmp += nextToken.data;
          anOffset = 2;
        }
        Operators theOp=Helpers::toOperator(tmp);
        if (notFlag) theOp = Helpers::toContraryOperator(theOp);
        aTokenizer.next(anOffset);
        if((theResult=parseOperand(aTokenizer,anEntity,theRHS))) {
          Logical theLogic = Logical::unknown_op;
          if (Keywords::and_kw == aTokenizer.current().keyword ||
              Keywords::or_kw == aTokenizer.current().keyword) {
            theLogic = Helpers::toLogical(aTokenizer.current().data);
            aTokenizer.next();
          }
          if(validateOperands(theLHS, theRHS, anEntity)) {
            add(new Expression(theLHS, theOp, theRHS, theLogic));
            if(aTokenizer.skipIf(semicolon)) {
              break;
            }
          }
          else theResult.error=syntaxError;
        }
      }
    }
    else theResult.error=syntaxError;
  }
  return theResult;
}

}

