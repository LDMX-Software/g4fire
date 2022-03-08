#ifndef G4FIRE_USERSTACKINGACTION_H
#define G4FIRE_USERSTACKINGACTION_H

#include <vector>

#include "G4UserStackingAction.hh"

#include "g4fire/UserAction.h"

namespace g4fire {

/**
 * @brief Class implementing a user stacking action.
 */
class UserStackingAction : public G4UserStackingAction {
 public:
  /// Constructor
  UserStackingAction();

  /// Destructor
  virtual ~UserStackingAction() final override;

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
   * @param action  User action of type StackingAction
   */
  void registerAction(UserAction* stacking_action) {
    stacking_actions_.push_back(stacking_action);
  }

 private:
  /// Collection of user stacking actions
  std::vector<UserAction*> stacking_actions_;

};  // UserStackingAction

}  // namespace g4fire

#endif  // G4FIRE_USERSTACKINGACTION_H
