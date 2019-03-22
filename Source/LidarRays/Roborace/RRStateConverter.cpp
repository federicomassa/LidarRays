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

RRStateConverter::RRStateConverter(std::string track_file_path, EnvironmentParameters& env)
{
	std::ifstream in_file(track_file_path.c_str());
	LogFunctions::Require(in_file.is_open(), "RRStateConverter::RStateConverter", std::string("File ") + track_file_path + " not found.");

	track_file = new nlohmann::json;
	in_file >> *track_file;

	size_t start_index = 0;

	// ================ START ZONE ======================== //
	auto zones_json = track_file->find("Zone");
	LogFunctions::Require(zones_json->is_array(), "RRStateConverter::RRstateConverter", "Zone field in json must be an array");

	for (auto itr = zones_json->begin(); itr != zones_json->end(); itr++)
	{
		auto type = itr->find("Type");
		auto start = itr->find("Start");
		auto end = itr->find("End");
		auto id = itr->find("ID");

		LogFunctions::Require(type != itr->end(), "RRStateConverter::RRstateConverter", "Type field in zone must exist");
		LogFunctions::Require(type->is_number_integer(), "RRStateConverter::RRstateConverter", "Type field in zone must be an integer");

		LogFunctions::Require(start != itr->end(), "RRStateConverter::RRstateConverter", "Start field in zone must exist");
		LogFunctions::Require(start->is_number_integer(), "RRStateConverter::RRstateConverter", "Start field in zone must be an integer");

		LogFunctions::Require(end != itr->end(), "RRStateConverter::RRstateConverter", "End field in zone must exist");
		LogFunctions::Require(end->is_number_integer(), "RRStateConverter::RRstateConverter", "End field in zone must be an integer");

		LogFunctions::Require(id != itr->end(), "RRStateConverter::RRstateConverter", "ID field in zone must exist");
		LogFunctions::Require(id->is_number_integer(), "RRStateConverter::RRstateConverter", "ID field in zone must be an integer");

		Zone zone;
		zone.Type = type->get<int>();

		zone.Start = start->get<int>();
		zone.End = end->get<int>();
		zone.ID = id->get<int>();

		// Start point
		if (zone.Type == 0)
			start_index = zone.Start;

		zones.push_back(zone);
	}



	// ================ CENTER LINE CALCULATIONS ======================== //

	

	auto center_line_json = track_file->find("Centre");
	LogFunctions::Require(center_line_json->is_array(), "RRStateConverter::RRStateConverter", "Centre object must be an array");

	// Transform center line (x,y) into a curvilinear coordinate
	double s = 0;

	// From start index to end
	for (auto itr = center_line_json->begin() + start_index; itr != center_line_json->end(); itr++)
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

	// From beginning to start index
	for (auto itr = center_line_json->begin(); itr != center_line_json->begin() + start_index; itr++)
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

	// OTHER ZONES --> CORRECT INDEXES BASED ON STARTING POINT
	for (auto& zone : zones)
	{
		zone.Start -= start_index;

		if (zone.Start < 0)
			zone.Start += center_line.size();

		zone.End -= start_index;
		if (zone.End < 0)
			zone.End += center_line.size();
	}

	env.AddEntry("total_circuit_length", center_line_curv.back());

	LogFunctions::Require(center_line.size() == center_line_curv.size(), "RRStateConverter::RRstateConverter", "Bad calculation of center line curvilinear coordinates");

}

RRStateConverter::~RRStateConverter()
{
	if (track_file)
		delete track_file;
}

State RRStateConverter::Convert(const State& s, AgentParameters& par)
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
	par.AddEntry("next_index", 0.0);
	par.AddEntry("current_zone_type", 0.0);
	par.AddEntry("current_zone_id", 0.0);

	double converted_x = s("x")*0.01;
	double converted_y = -s("y")*0.01;
	double converted_v = s("v")*0.01;


	// TODO use previous point and limit the search on an interval of points around it. This is both inefficient and dangerous
	// if there are two track segments that are close to each other in (x,y) but far in s

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
	bool corrected = false;

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

		if (new_x > distance(previous_x, previous_y, next_x, next_y))
		{
			min_index++;
			if ((size_t)min_index == center_line.size())
				min_index = 0;
			
			corrected = true;

			continue;
		}
		else if (new_x < 0)
		{
			if (!corrected)
				LogFunctions::Error("RRStateConverter::Convert", "FIXME I thought this could not happen");
			else
				break;
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

	par("next_index") = min_index;

	par("current_zone_type") = -1;
	par("current_zone_id") = -1;

	int previous_index = min_index - 1;
	if (previous_index < 0)
		previous_index = int(center_line.size() - 1);

	// Check if inside a zone
	for (auto zone : zones)
	{
		// A car is in a zone if its previous index on the center line is within the indexes of the zone: [Z_start, Z_end[
		if (zone.End >= zone.Start)
		{
			if (previous_index >= zone.Start && previous_index < zone.End)
			{
				par.AddEntry("current_zone_length", center_line_curv.at(zone.End) - center_line_curv.at(zone.Start));
				par("current_zone_type") = (double)zone.Type;
				par("current_zone_id") = (double)zone.ID;
				break;
			}
		}
		else
		{
			if (previous_index >= zone.Start || previous_index < zone.End)
			{
				// When current zone loops the center line array, the length is TOTAL_CIRCUIT_LENGTH - (S_START - S_END), because start is ahead of end
				par.AddEntry("current_zone_length", center_line_curv.back() - (center_line_curv.at(zone.Start) - center_line_curv.at(zone.End)));
				par("current_zone_type") = (double)zone.Type;
				par("current_zone_id") = (double)zone.ID;
				break;
			}
		}
	}

	return out;
}