#include "EventAction.h"

#include <iostream>

#include <G4Event.hh>

namespace g4fire::g4user {

void EventAction::BeginOfEventAction(const G4Event *event) {
  // Call user event actions
  for (auto &event_action : event_actions_)
    event_action->BeginOfEventAction(event);
}

void EventAction::EndOfEventAction(const G4Event *event) {
  // Call user event actions
  for (auto &event_action : event_actions_)
    event_action->EndOfEventAction(event);
}

} // namespace g4fire::g4user
