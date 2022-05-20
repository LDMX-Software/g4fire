#ifndef G4FIRE_G4USER_STACKINGACTION_H
#define G4FIRE_G4USER_STACKINGACTION_H

#include <vector>

#include "G4UserStackingAction.hh"

#include "g4fire/user/Action.h"

namespace g4fire::g4user {

/**
 * @brief Class implementing a user stacking action.
 */
class StackingAction : public G4UserStackingAction {
 public:
  /// Constructor
  StackingAction() = default;

  /// Destructor
  virtual ~StackingAction() = default;

  /**
   * Classify a new track.
   * @param aTrack The track to classify.
   * @return The track classification.
   */
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track);

  /**
   * Invoked when there is a new stacking stage.
   */
  void NewStage();

  /**
   * Invoked for a new event.
   */
  void PrepareNewEvent();

  /**
   * Register a user action of type stacking action with this class.
   *
   * @param action   action of type StackingAction
   */
  void registerAction(user::Action* stacking_action) {
    stacking_actions_.push_back(stacking_action);
  }

 private:
  /// Collection of user stacking actions
  std::vector<user::Action*> stacking_actions_;

};  // StackingAction

}  // namespace g4fire

#endif  // G4FIRE_USERSTACKINGACTION_H
