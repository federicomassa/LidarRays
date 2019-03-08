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

class RuleMonitor
{
 private:
  const ActionManager& aMan;
  SocialRules* rules;

  /* every record is an action and its associated rules */
  std::vector<std::pair<ActionInfo, std::set<Rule> > > processedActions;
  
  void processActions(double time);
  void registerNewAction(ActionInfo);
 public:
  RuleMonitor(const ActionManager&);
 
  /* owns social rules object */
  ~RuleMonitor();
  
  /* take ownership */
  void setRules(SocialRules* sr) {
    if (sr == NULL)
      LogFunctions::Error("RuleMonitor::setRules", "Null pointer provided");
    
    rules = sr;
    buildRules();
  }

  /* build rules */
  void buildRules();

  /* get pointer to the action manager */
  const ActionManager* getActionManager() {return &aMan;}
  
  /* check vehicle behaviour based on actions detected by the action manager */
  void run(double time);

  /* accessor to processed actions */
  const std::vector<std::pair<ActionInfo, std::set<Rule> > >& getProcessedActions() const
    {return processedActions;}
};

#endif
