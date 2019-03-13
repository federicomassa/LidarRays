/*
  AUTHOR = Federico Massa
  EMAIL = fedemassa91@gmail.com
  DESCRIPTION = This is an abstract class representing a general action of a vehicle, which can be
                a lane change, an overtake, etc. The specific actions inherit from this class.
 */

#ifndef CONTESTANT_H
#define CONTESTANT_H

#include "ActionManager.h"
#include "RuleMonitor.h"
#include "AgentTrajectory.h"

class Contestant
{
	ActionManager aMan;
	RuleMonitor rMon;
	AgentTrajectory traj;

public:
	void setActionManager(const ActionManager& a)
	{
		aMan = a;
	}
	void setRuleMonitor(const RuleMonitor& r)
	{
		rMon = r;
	}
	const ActionManager& actionManager() const { return aMan; }
	const RuleMonitor& ruleMonitor() const { return rMon; }
	const AgentTrajectory& trajectory() const { return traj; }

	ActionManager& actionManager() { return aMan; }
	RuleMonitor& ruleMonitor() { return rMon; }
	AgentTrajectory& trajectory() { return traj; }

	std::string ID() const { return aMan.getID(); }
	void setID(std::string id)
	{
		traj.setID(id);
		aMan.setID(id);
	}
	void updateState(double time, State new_state);

	/* accessor to results */
	const TimedContainer<std::pair<ActionInfo, std::vector<Rule> > >& Results() const
	{
		return rMon.getProcessedActions();
	}
};


#endif
