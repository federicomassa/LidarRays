#include "RRStateConverter.h"
#include "State.h"
#include "LogFunctions.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <cmath>

double RRStateConverter::distance(double x1, double y1, double x2, double y2)
{
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

RRStateConverter::RRStateConverter(std::string track_file_path)
{
	std::ifstream in_file(track_file_path.c_str());
	LogFunctions::Require(in_file.is_open(), "RRStateConverter::RStateConverter", std::string("File ") + track_file_path + " not found.");

	track_file = new nlohmann::json;
	in_file >> *track_file;

	auto center_line_json = track_file->find("Centre");
	LogFunctions::Require(center_line_json->is_array(), "RRStateConverter::RRStateConverter", "Centre object must be an array");

	// Transform center line (x,y) into a curvilinear coordinate
	double s = 0;

	for (auto itr = center_line_json->begin(); itr != center_line_json->end(); itr++)
	{
		std::vector<double> point = itr->get <std::vector<double> >();
		LogFunctions::Require(point.size() == 2, "RRStateConverter::RRStateConverter", "Center line must be an array of 2 points");

		if (center_line.size() == 0)
		{
			// First element in curvilinear coordinates is origin
			center_line_curv.push_back(0.0);
		}
		else
		{
			// Other elements are euclidean difference with respect to previous point
			double deltaS = distance(point[0], point[1], center_line.back().first, center_line.back().second);

			s += deltaS;
			center_line_curv.push_back(s);
		}

		center_line.push_back(std::make_pair(point[0], point[1]));

		
	}

	LogFunctions::Require(center_line.size() == center_line_curv.size(), "RRStateConverter::RRstateConverter", "Bad calculation of center line curvilinear coordinates");
}

RRStateConverter::~RRStateConverter()
{
	if (track_file)
		delete track_file;
}

State RRStateConverter::Convert(const State& s)
{
	LogFunctions::Require(s.Contains("x"), "RRStateConverter::Convert", "Input state must be (x,y,v)");
	LogFunctions::Require(s.Contains("y"), "RRStateConverter::Convert", "Input state must be (x,y,v)");
	LogFunctions::Require(s.Contains("v"), "RRStateConverter::Convert", "Input state must be (x,y,v)");

	State out;
	// s = curvilinear coordinate along center line
	// y = offtrack with respect to center line (y > 0 to the left of the track, direction specified in json)
	// v = velocity module
	out.AddStateVariable("s");
	out.AddStateVariable("y");
	out.AddStateVariable("v");
	out.AddStateVariable("next_index");

	double converted_x = s("x")*0.01;
	double converted_y = -s("y")*0.01;
	double converted_v = s("v")*0.01;


	// Index on center line of minimum distance
	int min_index = -1;
	double min_dist = 1E9;

	int curr_index = 0;
	for (const auto& point : center_line)
	{
		double d = distance(converted_x, converted_y, point.first, point.second);
		if (d < min_dist)
		{
			min_dist = d;
			min_index = curr_index;
		}
		curr_index++;
	}

	LogFunctions::Require(min_index != -1, "RRStateConverter::Convert", "No points???");

	// Project car position on new reference frame 
	//    (origin on previous point on center line
	//     and x axis on segment between previous and current point on center line) 
	
	// Angle between the (x,y) frame and new frame
	volatile double previous_x, previous_y, next_x, next_y, new_x, new_y;

	// Closest point might not be the next in curvilinear coordinates
	// If this happens, to ensure that you have found the two points of the center line
	// that make the projection of the car be within them, consider increasing min_index

	for (int iteration = 0; true; iteration++)
	{
		if (min_index > 0)
		{
			previous_x = center_line.at(min_index-1).first;
			previous_y = center_line.at(min_index-1).second;
		}
		else
		{
			// Last element
			previous_x = center_line.back().first;
			previous_y = center_line.back().second;
		}

		next_x = center_line.at(min_index).first;
		next_y = center_line.at(min_index).second;

		volatile double phi = std::atan2(next_y - previous_y, next_x - previous_x);

		// Apply rotation of frames
		new_x = std::cos(phi)*(converted_x - previous_x) + std::sin(phi)*(converted_y - previous_y);
		new_y = -std::sin(phi)*(converted_x - previous_x) + std::cos(phi)*(converted_y - previous_y);

		LogFunctions::Require(iteration < 100, "RRStateConverter::Convert", "Unable to find proper points on center line");

		if (new_x < 0)
		{
			if (min_index > 0)
				min_index--;
			else
				min_index = int(center_line.size() - 1);

			continue;
		}
		else if (new_x > distance(previous_x, previous_y, next_x, next_y))
		{
			min_index++;
			if ((size_t)min_index == center_line.size())
				min_index = 0;

			continue;
		}
		else
			break;
	}

	if (min_index > 0)
		out("s") = center_line_curv.at(min_index-1) + new_x;
	else
		out("s") = center_line_curv.back() + new_x;

	out("y") = new_y;
	out("v") = converted_v;

	out("next_index") = min_index;

	return out;
}