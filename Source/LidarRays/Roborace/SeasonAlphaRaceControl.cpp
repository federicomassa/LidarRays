// Fill out your copyright notice in the Description page of Project Settings.

#include "SeasonAlphaRaceControl.h"

namespace SeasonAlpha
{
	struct ContestantPosition
	{
		std::string ID;
		double s;

		ContestantPosition(std::string id, double _s)
		{
			ID = id;
			s = _s;
		}

		bool operator<(const ContestantPosition& pos)
		{
			if (s < pos.s)
				return true;
			
			return false;
		}
	};
}

SeasonAlphaRaceControl::SeasonAlphaRaceControl()
{
}

void SeasonAlphaRaceControl::Evaluate(double time)
{
	SetRacePositions();

	for (auto& contestant : Contestants())
	{
		auto results = contestant.Results();
		for (const auto& action : results.ActionHistory)
		{
			if (action.name == "TriggerAttack" && action.endTime == time && action.status == ENDED)
			{
				Contestant* opponent = nullptr;

				for (auto& other : Contestants())
				{
					if (other.parameter("race_position") == contestant.parameter("race_position") - 1)
					{
						opponent = &other;
						break;
					}
				}

				LogFunctions::Require(opponent != nullptr, "SeasonAlphaRaceControl::Evaluate", "FIXME I thought this could not happen. Who was it overtaking?");

				// Set attacker and defender
				contestant.parameter("overtaking_status") = 1.0;
				opponent->parameter("overtaking_status") = 2.0;
			}
		}
	}
}

void SeasonAlphaRaceControl::SetRacePositions()
{
	std::vector<SeasonAlpha::ContestantPosition> positions;

	for (const auto& contestant : Contestants())
	{
		SeasonAlpha::ContestantPosition pos(contestant.ID(), contestant.trajectory().getTrajectory().latest().value()("s"));
		positions.push_back(pos);
	}

	std::sort(positions.begin(), positions.end());

	for (auto& contestant : Contestants())
	{
		for (int i = 0; i < positions.size(); i++)
		{
			if (contestant.ID() != positions[i].ID)
				continue;

			contestant.setParameter("race_position") = double(Contestants().size() - i);
		}
	}
}
