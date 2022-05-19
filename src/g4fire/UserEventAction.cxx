
#include "g4fire/UserEventAction.h"

#include <iostream>

#include "g4fire/TrackMap.h"
#include "g4fire/UserTrackingAction.h"

#include "G4Event.hh"

namespace g4fire {

void UserEventAction::BeginOfEventAction(const G4Event *event) {
  // Clear the global track map.
  UserTrackingAction::getUserTrackingAction()->getTrackMap()->clear();

  // Call user event actions
  for (auto &event_action : event_actions_)
    event_action->BeginOfEventAction(event);
}

void UserEventAction::EndOfEventAction(const G4Event *event) {
  // Call user event actions
  for (auto &event_action : event_actions_)
    event_action->EndOfEventAction(event);
}

} // namespace g4fire
