//
//  View.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.


#ifndef View_h
#define View_h


#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include "Config.hpp"

namespace ECE141 {

//completely generic view, which you will subclass to show information
class View {
public:
  virtual         ~View() {}
  virtual bool    show(std::ostream &aStream)=0;
  
  // 4 rows in set
  View& showRowsInSet(std::ostream &anOutput, size_t aSize) {
    anOutput << aSize << " rows in set ";
    return *this;
  }
  
  // close the timer and output the timestamp to stream
  View& addTimeStamp(std::ostream &anOutput) {
    auto timeElapsed = Config::getTimer().elapsed();
    anOutput << "(" << std::fixed << timeElapsed << " sec)\n";
    return *this;
  }
  
};

// Want to output: Query OK, 1 row affected (0.01 secs)
class ResultView : public View {
public:
  ResultView(size_t aCount=0)
  : count(aCount) {}
  
  virtual bool show(std::ostream &anOutput) override {
    anOutput << "Query OK, ";
//    anOutput << count << (1 < count ? " rows affected " : " row affected ");
    anOutput << count << (" rows affected ");
    addTimeStamp(anOutput);
    return true;
  }
protected:
  size_t    count;
};

}

#endif /* View_h */
