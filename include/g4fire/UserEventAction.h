#ifndef G4FIRE_USEREVENTACTION_H
#define G4FIRE_USEREVENTACTION_H

#include <vector>

#include "G4UserEventAction.hh"

#include "g4fire/UserAction.h"

class G4Event;

namespace g4fire {

/**
 * @brief Implementation of user event action hook
 */
class UserEventAction : public G4UserEventAction {
 public:
  /// Class constructor.
  UserEventAction() = default;

  /// Class destructor.
  virtual ~UserEventAction() = default;

  /**
   * Implementation of begin of event hook.
   *
   * @param event The Geant4 event.
   */
  void BeginOfEventAction(const G4Event* event);

  /**
   * Implementation of end of event hook.
   *
   * @param event The Geant4 event.
   */
  void EndOfEventAction(const G4Event* event);

  /**
   * Register a user action of type EventAction with this class.
   *
   * @param action  User action of type EventAction
   */
  void registerAction(UserAction* event_action) {
    event_actions_.push_back(event_action);
  }

 private:
  std::vector<UserAction*> event_actions_;

};  // UserEventAction

}  // namespace g4fire

#endif  // G4FIRE_USEREVENTACTION_H
