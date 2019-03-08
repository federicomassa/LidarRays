// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/AutomationTest.h"
#include "TestLogs.h"
#include "RaceControl.h"
#include "State.h"
#include <set>

DEFINE_LOG_CATEGORY(RaceTestsLog);

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRaceTest, "Tazio.RaceTests.Race", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRulesTest, "Tazio.RaceTests.Rule", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

using namespace Race;

bool FRaceTest::RunTest(const FString& Parameters)
{
	UE_LOG(RaceTestsLog, Display, TEXT("Starting Race Test..."));

	RaceControl Race;

	bool caught = false;
	try
	{
		Race.RegisterContestant("Tazio");
	}
	catch (UninitializedException&)
	{
		caught = true;
	}

	TestTrue("Caught exception", caught);


	return true;
}

bool FRulesTest::RunTest(const FString& Parameters)
{
	UE_LOG(RaceTestsLog, Display, TEXT("Starting Rules Test..."));
	return true;
}


#endif