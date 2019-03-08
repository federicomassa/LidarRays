#include "TestRules.h"
#include "Event.h"
#include "SubEvent.h"
#include "ruleFunctions.h"
#include <string>

void TestRules::build()
{
  addSafetyRules();
}

void TestRules::addSafetyRules()
{
  std::set<SubEvent> seList;
  std::set<Event> eList;
    
  SubEvent sSafety("Sub_1", &safety_1, nullptr, SubEvent::EvalMode::OR, "Safety description");
  seList.insert(sSafety);

  /* Transfer subevent ownership to SocialRules */
  Event eSafety(std::string("Safety event name"), seList, std::string("Safety event description"));
  eList.insert(eSafety);

  addRule("Safety Rule", eList, "You are not keeping the safety distance from the vehicle in front of you", CONTINUOUS);

  /* clear for re-use */
  seList.clear();
  eList.clear();
}



TestRules::~TestRules()
{
}
