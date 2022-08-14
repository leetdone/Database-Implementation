//
//  Filters.hpp
//  RGAssignment5
//
//  Created by rick gessner on 4/4/21.
//  Copyright © 2021 rick gessner. All rights reserved.


#ifndef Filters_h
#define Filters_h

#include <memory>
#include <string>
#include "Errors.hpp"
#include "Tokenizer.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {

class Entity;

struct Operand {
  Operand() {}
  Operand(std::string &aName, TokenType aType, Value &aValue, size_t anId=0)
  : ttype(aType), dtype(DataTypes::varchar_type), name(aName), value(aValue), entityId(anId) {}
  
  TokenType   ttype; //is it a field, or const (#, string)...
  DataTypes   dtype;
  std::string name;  //attr name
  Value       value;
  size_t      entityId;
};

//------------------------------------------------------------------------
struct Expression {
  Operand     lhs;   //id
  Operand     rhs;   //usually a constant; maybe a field...
  Operators   op;    //=     //users.id=books.author_id
  Logical     logic; //and, or, not...
  
  Expression() {}
  Expression(Operand &aLHSOperand, Operators anOp, Operand &aRHSOperand, Logical aLogic=Logical::no_op)
  : lhs(aLHSOperand), rhs(aRHSOperand),op(anOp), logic(aLogic) {}
  Expression(const Expression& aCopy) = default;
  bool operator()(KeyValues &aList);
};

using Expressions = std::vector<std::unique_ptr<Expression>>;

//------------------------------------------------------------------------
class Filters {
public:
  Filters();
  ~Filters();
  
  size_t        getCount() const {return expressions.size();}
  bool          matches(KeyValues &aList) const;
  Filters&      add(Expression *anExpression);
  StatusResult  parse(Tokenizer &aTokenizer, Entity &anEntity);
protected:
  Expressions   expressions;
};

}

#endif /* Filters_h */
