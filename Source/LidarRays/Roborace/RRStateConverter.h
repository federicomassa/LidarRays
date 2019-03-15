// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateConverter.h"
#include "nlohmann/json_fwd.hpp"
#include <vector>
#include <utility>

class State;

class RRStateConverter : public StateConverter
{
	nlohmann::json* track_file = nullptr;

	// Center line in (x,y)
	std::vector<std::pair<double, double> > center_line;

	// Center line in s
	std::vector<double> center_line_curv;

	static double distance(double x1, double y1, double x2, double y2);
public:
	RRStateConverter(std::string track_file_path);
	~RRStateConverter();

	State Convert(const State&);
};
