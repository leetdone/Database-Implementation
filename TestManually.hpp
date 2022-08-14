//
//  TestManually.hpp
//  RGAssignement1
//
//  Created by rick gessner on 2/26/21.
//

#ifndef TestManually_h
#define TestManually_h

#include "Application.hpp"
#include "Errors.hpp"
#include <sstream>


void showError(ECE141::StatusResult &aResult, std::ostream &anOutput) {

  static std::map<ECE141::Errors, std::string> theErrorMessages = {
    {ECE141::illegalIdentifier, "Illegal identifier"},
    {ECE141::unknownIdentifier, "Unknown identifier"},
    {ECE141::databaseExists, "Database exists"},
    {ECE141::tableExists, "Table Exists"},
    {ECE141::syntaxError, "Syntax Error"},
    {ECE141::unknownCommand, "Unknown command"},
    {ECE141::unknownDatabase,"Unknown database"},
    {ECE141::unknownTable,   "Unknown table"},
    {ECE141::unknownError,   "Unknown error"},
    {ECE141::noDatabaseSpecified,   "Use a database before create"}
  };

  std::string theMessage="Unknown Error";
  if(theErrorMessages.count(aResult.error)) {
    theMessage=theErrorMessages[aResult.error];
  }
  anOutput << "Error (" << aResult.error << ") " << theMessage << "\n";
}


bool doManualTesting() {
  ECE141::Application   theApp(std::cout);
  ECE141::StatusResult  theResult{};

  std::string theUserInput;
  bool running=true;
  do {
    std::cout << "\n> ";
    if(std::getline(std::cin, theUserInput)) {
      if(theUserInput.length()) {
        std::stringstream theStream(theUserInput);
        theResult=theApp.handleInput(theStream);
        if(theResult==ECE141::userTerminated) {
          running=false;
        }
        else if(!theResult) {
          showError(theResult, std::cout);
        }
      }
    }
  }
  while (running);
  return theResult;
}

#endif /* TestManually_h */
