#ifndef G4FIRE_G4USER_RUNACTION_H
#define G4FIRE_G4USER_RUNACTION_H

#include <vector>

#include <G4UserRunAction.hh>

#include "g4fire/user/Action.h"

class G4Run;

namespace g4fire::g4user {

/**
 * @brief Implementation of user run action hook
 */
class RunAction : public G4UserRunAction {
 public:
  /// Class constructor.
  RunAction() = default;

  /// Class destructor.
  virtual ~RunAction() = default;

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
   * @param action   action of type RunAction
   */
  void attach(user::Action* run_action) {
    run_actions_.push_back(run_action);
  }

 private:
  std::vector<user::Action*> run_actions_;

};  // RunAction
}  // namespace g4fire

#endif  // G4FIRE_USERRUNACTION_H
