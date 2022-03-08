#include "g4fire/UserStackingAction.h"

namespace g4fire {

G4ClassificationOfNewTrack
UserStackingAction::ClassifyNewTrack(const G4Track *track) {
  // Default value of a track is fUrgent.
  G4ClassificationOfNewTrack current_track_class =
      G4ClassificationOfNewTrack::fUrgent;

  // Get proposed new track classification from this plugin.
  for (auto &stacking_action : stacking_actions_) {
    // Get proposed new track classification from this plugin.
    G4ClassificationOfNewTrack newTrackClass =
        stacking_action->ClassifyNewTrack(track, current_track_class);

    // Only set the current classification if the plugin changed it.
    if (newTrackClass != current_track_class)
      current_track_class = newTrackClass;
  }

  return current_track_class;
}

void UserStackingAction::NewStage() {
  for (auto &stacking_action : stacking_actions_)
    stacking_action->NewStage();
}

void UserStackingAction::PrepareNewEvent() {
  for (auto &stacking_action : stacking_actions_)
    stacking_action->PrepareNewEvent();
}
} // namespace g4fire
