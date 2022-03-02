
#include "g4fire/UserEventAction.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <iostream>

/*~~~~~~~~~~~~~*/
/*   g4fire   */
/*~~~~~~~~~~~~~*/
#include "g4fire/RunManager.h"
#include "g4fire/TrackMap.h"
#include "g4fire/UserTrackingAction.h"

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4Event.hh"
#include "G4RunManager.hh"

namespace simcore {

void UserEventAction::BeginOfEventAction(const G4Event* event) {
  // Clear the global track map.
  UserTrackingAction::getUserTrackingAction()->getTrackMap()->clear();

  // Call user event actions
  for (auto& eventAction : eventActions_)
    eventAction->BeginOfEventAction(event);
}

void UserEventAction::EndOfEventAction(const G4Event* event) {
  // Call user event actions
  for (auto& eventAction : eventActions_) eventAction->EndOfEventAction(event);
}

}  // namespace simcore
