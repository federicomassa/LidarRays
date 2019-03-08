#include "TestAction.h"

const std::string TestAction::actionName = "TEST";

TestAction::TestAction()
{}

TestAction::~TestAction() {}

void TestAction::initRuleCategories()
{
  addRuleCategory("TestCategory");
}

bool TestAction::triggerCondition()
{  
	return false;
}

bool TestAction::endCondition()
{ 
  return false;
}

bool TestAction::abortCondition()
{
  return false;
}
