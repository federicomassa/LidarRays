/* 
   AUTHOR = Federico Massa
   EMAIL = fedemassa91@gmail.com
 */

#pragma once

#include "Action.h"
#include "State.h"

#include <string>

class TestAction : public Action
{

 public:
  TestAction();
  ~TestAction();

  bool triggerCondition();
  bool endCondition();
  bool abortCondition();
  
  void initRuleCategories();
  std::string name() const {return actionName;}
  static const std::string actionName;
};
