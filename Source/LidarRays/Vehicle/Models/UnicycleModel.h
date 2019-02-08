// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleModel.h"

class UnicycleModel : public VehicleModel
{
public:

	void initModel() override;
	void closeModel() override;

	void requestControl(std::map<std::string, double> controlRequest) override;
	void executeModel(double DeltaTime) override;

	std::map<std::string, double> controlsToModel(const std::map<std::string, double>& inControl) const override;
	std::map<std::string, double> statesToModel(const std::map<std::string, double>& inState) const override;

	std::map<std::string, double> controlsToWorld(const std::map<std::string, double>& inControl) const override;
	std::map<std::string, double> statesToWorld(const std::map<std::string, double>& inState) const override;
};