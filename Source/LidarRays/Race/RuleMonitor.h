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
#include "TimedContainer.h"
#include "EnvironmentParameters.h"
#include "Properties.h"
#include <vector>

#include <utility>

class AgentTrajectory;

class RuleMonitor
{
 private:
  std::shared_ptr<SocialRules> rules;

  TimedContainer<EnvironmentParameters> env_params;
  Properties properties;

  /* every record is an action and its associated rules */
  TimedContainer<std::pair<ActionInfo, std::vector<Rule> > > processedActions;

  /* build rules */
  void buildRules();

 public:
  RuleMonitor();
 
  /* owns social rules object */
  ~RuleMonitor();
  
  /* take ownership */
  void setRules(std::shared_ptr<SocialRules> sr) {
    if (sr == NULL)
      LogFunctions::Error("RuleMonitor::setRules", "Null pointer provided");
    
    rules = sr;
    buildRules();
  }

  // Update environment parameters
  void UpdateEnvironmentParameters(double time, const EnvironmentParameters& params);

  void SetProperties(const Properties& prop);
  
  /* check vehicle behaviour based on actions detected by the action manager */
  void run(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates, const ActionManager* aMan);

  /* accessor to processed actions */
  const TimedContainer<std::pair<ActionInfo, std::vector<Rule> > >& getProcessedActions() const
    {return processedActions;}
};

#endif
