//
//  DBProcessor.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <map>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"

namespace ECE141 {

class Application;
class SQLProcessor;

class DBProcessor : public CmdProcessor {
public:
  DBProcessor(std::ostream &anOutput, Application* anApp);
  ~DBProcessor();
  DBProcessor(const DBProcessor& aCopy) = delete;
  
  CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
  Statement*    makeStatement(Tokenizer &aTokenizer, [[maybe_unused]]StatusResult &aResult) override;
  StatusResult  run(Statement *aStmt) override;
protected:
  Application   *app;
  SQLProcessor  *sqlProc;
};

}
#endif /* DBProcessor_hpp */
