/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Test set of social rules
*/

#ifndef SEASON_ALPHA_RULES_H
#define SEASON_ALPHA_RULES_H

#include "SocialRules.h"

class SeasonAlphaRules : public SocialRules
{
 public:
  SeasonAlphaRules() {}
  ~SeasonAlphaRules();
  void build();
  void addSafetyRules();
  void addTriggerRules();
};

#endif
