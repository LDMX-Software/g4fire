#include "g4fire/user/Action.h"

namespace g4fire::user {

Action::Action(const fire::config::Parameters& p) {}
void Action::store(fire::Event&) {}
void Action::BeginOfEventAction(const G4Event*) {}
void Action::EndOfEventAction(const G4Event*) {}
void Action::BeginOfRunAction(const G4Run*) {}
void Action::EndOfRunAction(const G4Run*) {}
void Action::PreUserTrackingAction(const G4Track*) {}
void Action::PostUserTrackingAction(const G4Track*) {}
void Action::stepping(const G4Step*) {}
G4ClassificationOfNewTrack Action::ClassifyNewTrack(const G4Track*, 
    const G4ClassificationOfNewTrack& cl) { 
  return cl; 
}
void Action::NewStage() {}
void Action::PrepareNewEvent() {}

UserEventInformation* Action::getEventInfo() const {
  return static_cast<UserEventInformation*>(G4EventManager::GetEventManager()->GetCurrentEvent()->GetUserInformation());
}

}