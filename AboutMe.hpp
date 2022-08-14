//
//  Created by rick gessner on 3/23/22.
//

#ifndef AboutMe_hpp
#define AboutMe_hpp

#include <stdio.h>
#include <string>

namespace ECE141 {
  
  class AboutMe {
  public:
    AboutMe() : name("Lindong Ye"), pid("A59012957") {} 
    
    const std::string& getPID() const {return pid;}
    const std::string& getName() const {return name;}

  protected:
    std::string name;
    std::string pid;
  };

}

#endif /* about_me */
