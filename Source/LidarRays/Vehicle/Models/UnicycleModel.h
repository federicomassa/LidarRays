// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleModel.h"

class UnicycleModel : public VehicleModel
{
public:

	void initModel() override;
	void closeModel() override;

	void requestControl(std::map<std::string, float> controlRequest) override;
	void executeModel(float DeltaTime) override;

	std::map<std::string, float> controlsToModel(const std::map<std::string, float>& inControl) const override;
	std::map<std::string, float> statesToModel(const std::map<std::string, float>& inState) const override;

	std::map<std::string, float> controlsToWorld(const std::map<std::string, float>& inControl) const override;
	std::map<std::string, float> statesToWorld(const std::map<std::string, float>& inState) const override;
};