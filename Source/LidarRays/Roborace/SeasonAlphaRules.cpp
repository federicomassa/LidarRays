#include "SeasonAlphaRules.h"
#include "Event.h"
#include "SubEvent.h"
#include "RuleFunctions.h"
#include <string>

void SeasonAlphaRules::build()
{
  addSafetyRules();
}

void SeasonAlphaRules::addSafetyRules()
{
  std::set<SubEvent> seList;
  std::set<Event> eList;
    
  SubEvent sSafety("Sub_1", &safety, nullptr, SubEvent::EvalMode::OR, "Safety description");
  seList.insert(sSafety);

  /* Transfer subevent ownership to SocialRules */
  Event eSafety(std::string("Safety event name"), seList, std::string("Safety event description"));
  eList.insert(eSafety);

  addRule("Safety", eList, "Safety distance", CONTINUOUS);

  /* clear for re-use */
  seList.clear();
  eList.clear();
}

void SeasonAlphaRules::addTriggerRules()
{
	std::set<SubEvent> seList;
	std::set<Event> eList;

	SubEvent sSafety("Sub_1", &safety, nullptr, SubEvent::EvalMode::OR, "Safety description");
	seList.insert(sSafety);

	/* Transfer subevent ownership to SocialRules */
	Event eSafety(std::string("Safety event name"), seList, std::string("Safety event description"));
	eList.insert(eSafety);

	addRule("Safety", eList, "Safety distance", CONTINUOUS);

	/* clear for re-use */
	seList.clear();
	eList.clear();
}



SeasonAlphaRules::~SeasonAlphaRules()
{
	
}
