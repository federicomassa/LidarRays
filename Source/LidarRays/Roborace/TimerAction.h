/* 
   AUTHOR = Federico Massa
   EMAIL = fedemassa91@gmail.com
 */

#pragma once

#include "Action.h"
#include "State.h"

#include <string>

class TimerAction : public Action
{
	double lastEndTime = -1;
 public:
  /* Trigger condition specifies the conditions that start the action */
  bool triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates) override;
  /* End condition specifies the conditions that end the action */
  bool endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates) override;
  /* Abort condition specifies the conditions that stop the listener of this action */
  bool abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates) override;
  
  void initRuleCategories() override;
  std::string name() const override {return actionName;}
  static const std::string actionName;
};
