#ifndef G4FIRE_G4USER_STEPPINGACTION_H
#define G4FIRE_G4USER_STEPPINGACTION_H

#include <vector>

#include "G4UserSteppingAction.hh"

#include "g4fire/user/Action.h"

namespace g4fire::g4user {

/**
 * @brief Implements the Geant4 user stepping action.
 */
class SteppingAction : public G4UserSteppingAction {
 public:
  /// Destructor
  ~SteppingAction() final override = default;

  /**
   * Callback used to process a step.
   *
   * @param step The Geant4 step.
   */
  void UserSteppingAction(const G4Step *step) final override;

  /**
   * Register a user action of type SteppingAction with this class.
   *
   * @param action  User action of type SteppingAction
   */
  void attach(user::Action *stepping_action) {
    stepping_actions_.push_back(stepping_action);
  }

 private:
  /// Collection of user stepping actions
  std::vector<user::Action *> stepping_actions_;

}; // SteppingAction
} // namespace g4fire::g4user

#endif // G4FIRE_G4USER_STEPPINGACTION_H
