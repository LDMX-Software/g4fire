#ifndef G4FIRE_USERRUNACTION_H
#define G4FIRE_USERRUNACTION_H

#include <vector>

#include "G4UserRunAction.hh"

#include "g4fire/UserAction.h"

class G4Run;

namespace g4fire {

/**
 * @brief Implementation of user run action hook
 */
class UserRunAction : public G4UserRunAction {
 public:
  /// Class constructor.
  UserRunAction() = default;

  /// Class destructor.
  virtual ~UserRunAction() = default;

  /**
   * Implementation of begin run hook.
   * @param run The current Geant4 run info.
   */
  void BeginOfRunAction(const G4Run* run);

  /**
   * Implementation of end run hook.
   * @param run The current Geant4 run info.
   */
  void EndOfRunAction(const G4Run* run);

  /**
   * Register a user action of type RunAction with this class.
   *
   * @param action  User action of type RunAction
   */
  void registerAction(UserAction* run_action) {
    run_actions_.push_back(run_action);
  }

 private:
  std::vector<UserAction*> run_actions_;

};  // UserRunAction
}  // namespace g4fire

#endif  // G4FIRE_USERRUNACTION_H
