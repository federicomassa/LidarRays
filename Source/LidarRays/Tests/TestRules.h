/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Test set of social rules
*/

#ifndef TEST_RULES_H
#define TEST_RULES_H

#include "SocialRules.h"

class TestRules : public SocialRules
{
 public:
  TestRules() {}
  ~TestRules();
  void build();
  void addSafetyRules();
};

#endif
