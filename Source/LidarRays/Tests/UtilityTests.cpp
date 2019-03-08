// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/AutomationTest.h"
#include "TestLogs.h"
#include "Timeseries.h"
#include <fstream>

DEFINE_LOG_CATEGORY(UtilityTestsLog);

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUtilityTest, "Tazio.UtilityTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUtilityTest::RunTest(const FString& Parameters)
{
	UE_LOG(UtilityTestsLog, Display, TEXT("Starting Utility Tests..."));

	Timeseries<double> ts;
	TestEqual("Timeseries test size", ts.size(), 0);
	ts.insert(0.0, 0.0);
	ts.insert(1.0, 1.0);
	ts.insert(0.5, 0.5);
	ts.insert(0.5, 0.5);
	ts.insert(-0.5, -0.5);
	TestEqual("Timeseries test insert", ts.size(), 5);

	int count = 0;
	TestTrue(FString::Printf(TEXT("Timeseries: line %d"), __LINE__), ts[count].time() == -0.5 && ts[count].time() == ts[count].value());
	count++;
	TestTrue(FString::Printf(TEXT("Timeseries: line %d"), __LINE__), ts[count].time() == 0.0 && ts[count].time() == ts[count].value());
	count++;
	TestTrue(FString::Printf(TEXT("Timeseries: line %d"), __LINE__), ts[count].time() == 0.5 && ts[count].time() == ts[count].value());
	count++;
	TestTrue(FString::Printf(TEXT("Timeseries: line %d"), __LINE__), ts[count].time() == 0.5 && ts[count].time() == ts[count].value());
	count++;
	TestTrue(FString::Printf(TEXT("Timeseries: line %d"), __LINE__), ts[count].time() == 1.0 && ts[count].time() == ts[count].value());
	count++;

	ts.clear();
	TestEqual("Timeseries test clear", ts.size(), 0);

	return true;
}

#endif