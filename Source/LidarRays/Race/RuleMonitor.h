/* 
   AUTHOR = Federico Massa
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Each monitor has a RuleMonitor object that checks if the actions detected by the ActionManager are
   compatible with the set of social rules.
*/

#ifndef RULE_MONITOR_H
#define RULE_MONITOR_H

#include "State.h"
#include "SocialRules.h"
#include "ActionManager.h"
#include <vector>

#include <utility>

class AgentTrajectory;

class RuleMonitor
{
 private:
  const ActionManager& aMan;
  std::shared_ptr<SocialRules> rules;

  /* every record is an action and its associated rules */
  std::vector<std::pair<ActionInfo, std::set<Rule> > > processedActions;

  // Processed rules
  std::set<Rule> processedRules;

  void processActions(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates);
  void registerNewAction(ActionInfo);
  /* build rules */
  void buildRules();

 public:
  RuleMonitor(const ActionManager&);
 
  /* owns social rules object */
  ~RuleMonitor();
  
  /* take ownership */
  void setRules(std::shared_ptr<SocialRules> sr) {
    if (sr == NULL)
      LogFunctions::Error("RuleMonitor::setRules", "Null pointer provided");
    
    rules = sr;
    buildRules();
  }



  /* get pointer to the action manager */
  const ActionManager* getActionManager() {return &aMan;}
  
  /* check vehicle behaviour based on actions detected by the action manager */
  void run(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates);

  /* accessor to processed actions */
  const std::vector<std::pair<ActionInfo, std::set<Rule> > >& getProcessedActions() const
    {return processedActions;}


  /* accessor to processed rules */
  const std::set<Rule>& getProcessedRules() const
  {
	  return processedRules;
  }
};

#endif
