#ifndef G4FIRE_USTEPPINGACTION_H
#define G4FIRE_USTEPPINGACTION_H

#include <vector>

#include "G4UserSteppingAction.hh"

#include "g4fire/UserAction.h"

namespace g4fire {

/**
 * @brief Implements the Geant4 user stepping action.
 */
class USteppingAction : public G4UserSteppingAction {
 public:
  /// Destructor
  ~USteppingAction() final override { ; }

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
  void registerAction(UserAction *stepping_action) {
    stepping_actions_.push_back(stepping_action);
  }

 private:
  /// Collection of user stepping actions
  std::vector<UserAction *> stepping_actions_;

}; // USteppingAction
} // namespace g4fire

#endif // G4FIRE_USTEPPINGACTION_H
