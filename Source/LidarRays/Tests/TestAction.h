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
	double triggerTime;
 public:
  TestAction();
  ~TestAction();

  bool triggerCondition(double time, const TimedContainer<Agent>* targetStates, const TimedContainer<AgentVector>* neighborsStates) override;
  bool endCondition(double time, const TimedContainer<Agent>* targetStates, const TimedContainer<AgentVector>* neighborsStates) override;
  bool abortCondition(double time, const TimedContainer<Agent>* targetStates, const TimedContainer<AgentVector>* neighborsStates) override;
  
  void initRuleCategories();
  std::string name() const {return actionName;}
  static const std::string actionName;
};
