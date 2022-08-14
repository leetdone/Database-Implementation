//
//  AboutUs.hpp
//  About your team members...
//
//  Created by rick gessner on 3/23/22.
//

#ifndef AboutMe_hpp
#define AboutMe_hpp

#include <string>
#include <optional>
#include <vector>

namespace ECE141 {

using StringOpt = std::optional<std::string>;

class AboutUs {
public:
  AboutUs() {
    records_pair.push_back(std::make_pair("Hongbo Qian", "Qianhongbo"));
    records_pair.push_back(std::make_pair("Lindong Ye", "leetdone"));
  }
  ~AboutUs()=default;
  
  size_t getTeamSize() {return records_pair.size();} //STUDENT: update this...
  
  StringOpt getName(size_t anIndex) const {
    if (anIndex < records_pair.size()) {
      return records_pair[anIndex].first;
    } else {
      return std::nullopt;
    }
  }
  StringOpt getGithubUsername(size_t anIndex) const {
    if (anIndex < records_pair.size()) {
      return records_pair[anIndex].second;
    } else {
      return std::nullopt;
    }
  }
protected:
  std::vector<std::pair<std::string, std::string>> records_pair;
};

}

#endif /* about_me */
