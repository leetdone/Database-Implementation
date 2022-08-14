//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <string>
#include <vector>
#include "BasicTypes.hpp"
#include "Storage.hpp"

namespace ECE141 {

class Attribute : public Storable {
public:
  Attribute(DataTypes aType=DataTypes::no_type);
  Attribute(const std::string& aName, DataTypes aType, uint32_t aSize=0);
  Attribute(const Attribute &aCopy) = default;
  virtual ~Attribute()=default;
  
  Attribute&            setName(const std::string &aName);
  Attribute&            setDataType(DataTypes aType);
  Attribute&            setSize(int aSize);
  Attribute&            setAutoIncrement(bool anAuto);
  Attribute&            setPrimaryKey(bool anAuto);
  Attribute&            setNullable(bool aNullable);
  Attribute&            setDefaultValue(Value& aValue);
  bool                  isValid(); //is this Entity valid?
  const std::string&    getName() const {return name;}
  DataTypes             getType() const {return type;}
  size_t                getSize() const {return size;}
  bool                  getDefaultStatus() const {return status;}
  Value                 getDefaultValue() const {return value;}
  Value                 toValue(const std::string &aValue) const;
  bool                  isPrimaryKey() const {return primary;}
  bool                  isNullable() const {return nullable;}
  bool                  isAutoIncrement() const {return autoIncrement;}
  virtual StatusResult  encode(std::ostream &anOutput) override;
  virtual StatusResult  decode(std::istream &anInput) override;
//  void                  initBlock(Block &aBlock);
protected:
  std::string           name;
  DataTypes             type;
  uint16_t              size : 10; //max=1000
  uint16_t              autoIncrement : 1;
  uint16_t              primary : 1;
  uint16_t              nullable : 1;
  bool                  status;
  Value                 value;
};

}


#endif /* Attribute_hpp */
