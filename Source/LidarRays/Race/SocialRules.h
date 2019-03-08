/* 
   AUTHOR = Federico Massa, Adriano Fagiolini,
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes a social rule. Many rules together constitutes the set of social rules that has to be monitored.
*/

#ifndef SOCIAL_RULES_H
#define SOCIAL_RULES_H

#include "Agent.h"
#include "Event.h"
#include "ExtValue.h"
#include "StateRegion.h"
#include "TimedContainer.h"
#include <string>
#include <set>

class SocialRules;

/* Type that describes when the rule has to be evaluated:
 ex: at trigger time or at each instant */
enum CheckMode{TRIGGER, CONTINUOUS};

class Rule {
  friend class SocialRules;

 private:
	 static int count;
	 int unique_id;

  /* This is the list of events that describes the forbidden situations for this rule category.
     In the case of the category "LeftLaneChange", an event could be like "left lane is occupied by someone else and you are not on 
     the maximum lane" */
  std::set<Event> eList;
  double lastCheckTime;
  bool processed;

  void updateProcessStatus(const double&, const double&);

  /* This is the name of the category that we want to set a rule on.
     This is very important as each rule set a list of situations in which
     this behaviour should not happen. An example of category could be "LeftLaneChange".
    Each action, as recognized by the ActionManager, is possibly registered to several categories. */
  std::string category;
  
  std::string name;
  CheckMode mode;

  bool checkResult;
  StateRegion positiveArea;
  StateRegion negativeArea;
  
  void init(const std::string& cat, const std::set<Event>& eL, const std::string& n, const CheckMode& m);
  
 public:

  Rule();
  ~Rule();
  
  const std::set<Event>& getEventList() {return eList;}
  
  void check(const TimedContainer<Agent>& monitorStates, const TimedContainer<AgentVector>& neighStates, const double& triggerTime,
	     const double& endTime, double currentTime);
  
  bool isProcessed() const;
  
  bool operator==(const Rule& r) const
  {
    return (category == r.category &&
	    name == r.name &&
	    eList == r.eList);
	    
  }

  bool operator!=(const Rule& r) {return !((*this) == r);}

  bool operator<(const Rule& r) const { return (unique_id < r.unique_id); }

  const std::string& getCategory() const {return category;}
};

class SocialRules
{
  
 protected:
  std::set<Rule> rList;
  
  /* Add rule to the list */
  void addRule(const Rule& r) {rList.insert(r);}
  void addRule(const std::string& cat, const std::set<Event>& eL, const std::string& name, const CheckMode& mode = TRIGGER)
  {
    Rule r;
    r.init(cat, eL, name, mode);
    addRule(r);
  }

  /* FIXME Virtual template version of addRule if one wants to implement his version of check? */
  /* template <class Derived>
   addRule(const Derived& r)
  we would need not a List<Rule> but a RefList<Rule>, where RefList is a List that contains references without instantiating objects. */
  
  /* Accessor (by reference) */
  const std::set<Rule>& getRules() {return rList;}
  
  /* clear list */
  void purge() {rList.clear();}
  
 public:
  SocialRules() {}
  virtual ~SocialRules() {}
  /* build list of rules. The memory for the sub-events must be allocated here
   and will be freed by the destructor */
  virtual void build() = 0;

  std::set<Rule> createRulesList(const std::set<std::string>&);
  
};


#endif