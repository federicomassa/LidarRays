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

  /* Trigger condition specifies the conditions that start the action */
  bool triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates);
  /* End condition specifies the conditions that end the action */
  bool endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates);
  /* Abort condition specifies the conditions that stop the listener of this action */
  bool abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates);
  
  void initRuleCategories();
  std::string name() const {return actionName;}
  static const std::string actionName;
};
