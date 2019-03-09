/* 
   AUTHOR = Federico Massa
   DESCRIPTION = This class adiministrates the actions. It contains the history of the monitored vehicle's states,
   as well as the functions to manage the specific action listeners.
*/

#pragma once

#include "Agent.h"
#include <vector>
#include <memory>
#include <list>
#include "TimedContainer.h"
#include "Action.h"

class AgentTrajectory;

class ActionManager {

  friend class RuleMonitor;
 private:

	 std::string ID;
  /* List of actions that are being monitored. Populated during initialization. */
  std::set<std::shared_ptr<Action> > listeners;
  /* Keeps track of the actions done by the monitored vehicle. */
  std::vector<ActionInfo> history;

  /* Record an ended or aborted action */
  void recordAction(std::shared_ptr<Action>);

  /* Reset Action */
  void resetAction(std::shared_ptr<Action>);
  
  /* delete objects inside listeners list and purge it. */
  void clearListeners();

  /* delete objects inside history list and purge it. */
  void clearHistory();
  
 public:
  /* Constructor */
  ActionManager();

  /* clear memory */
  ~ActionManager();

  /* create listeners. */
  void init();

  /* Monitor vehicle's actions. */
  void run(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates);
  
  /* allocate new object into listeners list. */
  void addListener(std::shared_ptr<Action>);

  /* accessor to history */
  const std::vector<ActionInfo>& getHistory() const {return history;}

  /* get active actions */
  std::vector<ActionInfo> getActiveActions() const;

  void setID(std::string id) { ID = id; }
  std::string getID() const { return ID; }
};