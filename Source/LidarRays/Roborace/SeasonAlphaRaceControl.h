// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RaceControl.h"

class Action;
class SocialRules;
class State;
class StateConverter;

class SeasonAlphaRaceControl : public RaceControl
{
	void SetRacePositions();
public:
	SeasonAlphaRaceControl();

	void Evaluate(double time) override;
};
