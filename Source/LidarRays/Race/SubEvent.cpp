#include "SubEvent.h"
#include "StateRegion.h"
#include "LogFunctions.h"
#include "AgentTrajectory.h"

using namespace std;
using namespace LogFunctions;

SubEvent::SubEvent(const string& n, SingleEvaluationFcn fcn, const EvalMode& m, const string& descr)
{
	Require(fcn != nullptr, "SubEvent::SubEvent", "Cannot instantiate sub-event without logical condition");
	
	Require(m == SINGLE || m == NSINGLE, "SubEvent::SubEvent", "Single evaluating sub-events must have a SINGLE or NSINGLE evaluation mode");
	
	// It's a single evaluating sub-event
	areaFcn = nullptr;
	interactionFcn = nullptr;
	
	name = n;
	singleEvaluationFcn = fcn;
	mode = m;
	description = descr;
}

SubEvent::SubEvent(const string& n, InteractionFcn fcn, AreaFcn area, const EvalMode& m, const string& descr)
{
	Require(fcn != nullptr, "SubEvent::SubEvent", "Cannot instantiate sub-event without logical condition");
	
	Require(m == OR || m == NOR, "SubEvent::SubEvent", "Interaction sub-events must have a SINGLE or NSINGLE evaluation mode");	
	
	// It's an interaction sub-event
	singleEvaluationFcn = nullptr;
		
	name = n;
	interactionFcn = fcn;
	areaFcn = area;
	mode = m;
	description = descr;
}

SubEvent::SubEvent(const SubEvent& se)
{
	name = se.name;
	singleEvaluationFcn = se.singleEvaluationFcn;
	interactionFcn = se.interactionFcn;
	areaFcn = se.areaFcn;
	mode = se.mode;
	description = se.description;
}


bool SubEvent::operator<(const SubEvent& se) const
{
	return name < se.name;
}

bool SubEvent::operator==(const SubEvent& se) const
{
	bool equal = true;
	
	equal = equal && (name == se.name);
	return equal;
}

const string& SubEvent::GetName() const
{
	return name;
}

bool SubEvent::Evaluate(const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates, const TimedContainer<EnvironmentParameters>& env, const Properties& automatonProperties) const
{
	// A sub-event evaluates to true based on the logical condition, the sensor data available, the evaluation mode chosen in Automaton::DefineRules for this sub-event.
	
	// mode == OR/NOR means that the logical condition is evaluated with an OR/NOR for each external agent
	// mode == SINGLE/NSINGLE means that the logical condition (its negation in NSINGLE case) is evaluated on 
	// the single agent (usually self during simulation, possibly an observed agent in Observer module) 
	
	// FIXME only taking latest value
	EnvironmentParameters currEnv;
	if (env.size() != 0)
		currEnv = env.latest().value();
	

	
	bool result = false;
	
	// ==================== OR/NOR case ===================== //
	if (mode == OR || mode == NOR)
	{	

		// Initialize to neutral element of logical operation
		if (mode == OR)
			result = false;
		else if (mode == NOR)
			result = true;
		
		for (auto currOther = neighborsStates.begin(); currOther != neighborsStates.end();
			 currOther++)
			 {
			// WARNING Area fcn takes a trajectory but only consider latest value of other agent
				 if (mode == OR)
				 {
					 // If area fcn was linked to the sub-event, use it to verify that the agent lies inside that area, otherwise just use the logical condition					 
					 if (areaFcn == nullptr)
						result = result || interactionFcn(targetStates, *currOther, currEnv, automatonProperties);
					 else
						 result = result || (interactionFcn(targetStates, *currOther, currEnv, automatonProperties) && areaFcn(targetStates).Contains(currOther->getTrajectory().latest().value()));
				 }
				 else if (mode == NOR)
				 {
					// If area fcn was linked to the sub-event, use it to verify that the agent lies inside that area, otherwise just use the logical condition
					 if (areaFcn == nullptr)
						result = result && !interactionFcn(targetStates, *currOther, currEnv, automatonProperties);
					 else
						 result = result && !(interactionFcn(targetStates, *currOther, currEnv, automatonProperties) && areaFcn(targetStates).Contains(currOther->getTrajectory().latest().value()));
				 }
			 }
	}
	
	// ==================== SINGLE/NSINGLE case ===================== //
	else if (mode == SINGLE || mode == NSINGLE)
	{	
		result = singleEvaluationFcn(targetStates, currEnv, automatonProperties);
		
		if (mode == NSINGLE)
			result = !result;
	}
	
	return result;
}


