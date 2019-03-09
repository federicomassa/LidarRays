#ifndef SUBEVENT_H
#define SUBEVENT_H

#include <string>
#include <list>
#include <map>

#include "TimedContainer.h"
#include "EnvironmentParameters.h"
#include "Agent.h"
#include "Properties.h"

class StateRegion;
class AgentTrajectory;

class SubEvent
{
	std::string name;
	std::string description;
public:
	enum EvalMode {OR, NOR, SINGLE, NSINGLE};
private:
	EvalMode mode;
	typedef StateRegion (*AreaFcn) (const AgentTrajectory& targetStates);
	typedef bool (*InteractionFcn) (const AgentTrajectory& targetStates, const AgentTrajectory& otherStates, const EnvironmentParameters& env, const Properties& automatonProperties);
	typedef bool (*SingleEvaluationFcn) (const AgentTrajectory& targetStates, const EnvironmentParameters& env, const Properties& automatonProperties);
	
	
	// 'single' sub-events, i.e. sub-events that can be evaluated
	// only based on measurements of a single agent, must only
	// define singleEvaluationFcn, 'interaction' sub-events must
	// only define areaFcn and interactionFcn. This corresponds to
	// the two RegisterSubEvent functions in Automaton class
	AreaFcn areaFcn;
	InteractionFcn interactionFcn;
	SingleEvaluationFcn singleEvaluationFcn;
	
public:
	SubEvent(const std::string& n, SingleEvaluationFcn fcn, const EvalMode& m, const std::string& descr = "");
	
	SubEvent(const std::string& n, InteractionFcn fcn, AreaFcn area, const EvalMode& m, const std::string& descr = "");
	
	SubEvent(const SubEvent&);
	
	const std::string& GetName() const;
	
	// Used to create sets. NB two subevents are equal if they have the same name
	bool operator<(const SubEvent&) const;
	bool operator==(const SubEvent&) const;
	
	bool Evaluate(const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates, const TimedContainer<EnvironmentParameters>& env, const Properties& automatonProperties) const;
};

#endif
