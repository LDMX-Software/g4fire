#include "StackingAction.h"

namespace g4fire::g4user {

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track *track) {
  // Default value of a track is fUrgent.
  G4ClassificationOfNewTrack current_track_class =
      G4ClassificationOfNewTrack::fUrgent;

  // Get proposed new track classification from this plugin.
  for (auto &stacking_action : stacking_actions_) {
    // Get proposed new track classification from this plugin.
    current_track_class = 
        stacking_action->ClassifyNewTrack(track, current_track_class);
  }

  return current_track_class;
}

void StackingAction::NewStage() {
  for (auto &stacking_action : stacking_actions_)
    stacking_action->NewStage();
}

void StackingAction::PrepareNewEvent() {
  for (auto &stacking_action : stacking_actions_)
    stacking_action->PrepareNewEvent();
}
} // namespace g4fire
