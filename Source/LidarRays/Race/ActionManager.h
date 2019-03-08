/* 
   AUTHOR = Federico Massa
   DESCRIPTION = This class adiministrates the actions. It contains the history of the monitored vehicle's states,
   as well as the functions to manage the specific action listeners.
*/

#pragma once

#include "Agent.h"
#include <vector>
#include "TimedContainer.h"
#include "Action.h"

class ActionManager {

  friend class RuleMonitor;
 private:
  const int agentID;
  const int targetID;
  
  /* Keeps track of the states of the monitored vehicle across the prefixed time span FIXME where? */
  const TimedContainer<Agent>* targetStates;
  const TimedContainer<AgentVector>* neighborsStates;
  /* List of actions that are being monitored. Populated during initialization. */
  std::set<Action*> listeners;
  /* Keeps track of the actions done by the monitored vehicle. */
  std::vector<ActionInfo> history;

  /* Record an ended or aborted action */
  void recordAction(Action*);

  /* Reset Action */
  void resetAction(Action*);
  
  /* delete objects inside listeners list and purge it. */
  void clearListeners();

  /* delete objects inside history list and purge it. */
  void clearHistory();
  
 public:
  /* Constructor */
  ActionManager(const int& aID, const int& tID, const TimedContainer<Agent>*, const TimedContainer<AgentVector>*);

  /* clear memory */
  ~ActionManager();

  /* create listeners. */
  void init();

  /* Monitor vehicle's actions. */
  void run(double time);
  
  /* allocate new object into listeners list. */
  void addListener(Action*);

  /* accessor to history */
  const std::vector<ActionInfo>& getHistory() const {return history;}

  /* get active actions */
  std::vector<ActionInfo> getActiveActions() const;

  const int& getAgentID() const {return agentID;}
  const int& getTargetID() const {return targetID;}
};