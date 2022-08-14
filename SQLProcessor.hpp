//
//  SQLProcessor.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include "Application.hpp"
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {

class SQLProcessor : public CmdProcessor {
 public:
  SQLProcessor(std::ostream& anOutput, Application* anApp);
  virtual ~SQLProcessor();
  SQLProcessor(const SQLProcessor& aCopy) = delete;

  CmdProcessor* recognizes(Tokenizer& aTokenizer) override;
  Statement* makeStatement(Tokenizer& aTokenizer, [[maybe_unused]] StatusResult& aResult) override;
  StatusResult run(Statement* aStmt) override;

 protected:
  Statement* createTBStatementFactory(Database* aDatabase);
  Statement* dropTBStatementFactory(Database* aDatabase);
  Statement* showTBStatementFactory(Database* aDatabase);
  Statement* describeTBStatementFactory(Database* aDatabase);
  Statement* insertTBStatementFactory(Database* aDatabase);
  Statement* selectStatementFactory(Database* aDatabase);
  Statement* updateStatementFactory(Database* aDatabase);
  Statement* deleteStatementFactory(Database* aDatabase);
  Statement* showIndexesStatementFactory(Database* aDatabase);
  Statement* showIndexStatementFactory(Database* aDatabase);
    Statement* alertAddStatementFactory(Database* aDatabase);
    Statement* alertDropStatementFactory(Database* aDatabase);
  Application* app;
};

}  // namespace ECE141
#endif /* SQLProcessor_hpp */
