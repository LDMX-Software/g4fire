#include "TrackingAction.h"

#include <G4PrimaryParticle.hh>
#include <G4VUserPrimaryParticleInformation.hh>

#include <iostream>

namespace g4fire::g4user {

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
  // Activate user tracking actions
  for (auto& tracking_action : tracking_actions_)
    tracking_action->PreUserTrackingAction(track);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {
  // Activate user tracking actions
  for (auto& tracking_action : tracking_actions_)
    tracking_action->PostUserTrackingAction(track);
}

}  // namespace g4fire
