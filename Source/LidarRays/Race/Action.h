/*
  AUTHOR = Federico Massa
  EMAIL = fedemassa91@gmail.com
  DESCRIPTION = This is an abstract class representing a general action of a vehicle, which can be
                a lane change, an overtake, etc. The specific actions inherit from this class.
 */

#ifndef ACTION_H
#define ACTION_H

#include "Agent.h"
#include "TimedContainer.h"
#include <vector>
#include <string>
#include <list>

class AgentTrajectory;

enum ActionStatus {TRIGGERED, ENDED, ABORTED, INACTIVE};

struct ActionInfo
{
	double triggerTime = -1;
	double endTime = -1;
	ActionStatus status = INACTIVE;
	std::string name = "";

	/* Each action is registered to a series of ruleCategories of which they must to obey the rules.
   Ex. both the action "travel" and "overtake" are registered to the rule category "cruise", of which
   they should respect the rules. */
	std::set<std::string> ruleCategoryList;
};

class Action
{
 protected:
  void addRuleCategory(const std::string& cat) {info.ruleCategoryList.insert(info.ruleCategoryList.begin(), cat);}
 public:
  Action();
  Action(const Action&);

  ActionInfo info;
  
  /* Trigger condition specifies the conditions that start the action */
  virtual bool triggerCondition(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates) = 0;
  /* End condition specifies the conditions that end the action */
  virtual bool endCondition(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates) = 0;
  /* Abort condition specifies the conditions that stop the listener of this action */
  virtual bool abortCondition(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates) = 0;

  virtual ~Action() {};

  // Tick of action recognition loop, at time <time>.
  // Time should be in seconds
  virtual void listen(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates);

  // Add offset to trigger, end and abort time. Useful when 
  // actions are recognized after they actually started
  virtual double getTriggerOffset() const {return 0;}
  virtual double getEndOffset() const {return 0;}
  virtual double getAbortOffset() const {return 0;}
  
  virtual std::string name() const = 0;

  virtual void initRuleCategories() = 0;
  const std::set<std::string>& getRuleCategories() const {return info.ruleCategoryList;}
  
  /* contains a string describing the details of the recognized action, same as print() */
  std::string details();
};

std::string toString(const ActionStatus&);

#endif
