//
//  Entity.hpp
//  Assignment3
//
//  Created by rick gessner on 3/18/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "Attribute.hpp"
#include "Errors.hpp"
#include "Storage.hpp"

namespace ECE141 {

using AttributeOpt = std::optional<Attribute>;
using AttributeList = std::vector<Attribute>;

class Entity : public Storable {
public:
  explicit Entity(const std::string& aName);
  Entity(const Entity &aCopy) = default;
  virtual ~Entity() = default;

  Entity&               setName(const std::string& aName) {name = aName; return *this;}
  const std::string&    getName() const {return name;}
  const AttributeList&  getAttributes() const {return attributes;}
  uint32_t              getNextAutoIncrement() {return ++autoincr;}
  Entity&               addAttribute(Attribute &anAttribute);
  Entity&             dropAttribute(Attribute &anAttribute);
  Attribute*            getAttribute(const std::string &aName) const;
  const Attribute*      getPrimaryKey() const;
  // storable interface
  virtual StatusResult  encode(std::ostream &anOutput) override;
  virtual StatusResult  decode(std::istream &anInput) override;
  // used by insert statement
  StatusResult          validateFieldList(const StringList& aFieldList);
  StatusResult          validateValueList(const StringList& aFieldList, const StringList& aStrList, ValueList& aValueList);
//    Row& erase(const std::string &aKey){
//        if(data.count(aKey)){
//            data.erase(aKey);
//        }
//        return *this
//    };
protected:
  AttributeList         attributes;
  std::string           name;
  uint32_t              autoincr;  //auto_increment
};

}
#endif /* Entity_hpp */
