#ifndef G4FIRE_G4USER_EVENTACTION_H
#define G4FIRE_G4USER_EVENTACTION_H

#include <vector>

#include <G4UserEventAction.hh>

#include "g4fire/user/Action.h"

class G4Event;

namespace g4fire::g4user {

/**
 * @brief Implementation of user event action hook
 */
class EventAction : public G4UserEventAction {
 public:
  /// Class constructor.
  EventAction() = default;

  /// Class destructor.
  virtual ~EventAction() = default;

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
   * @param action   action of type EventAction
   */
  void registerAction(user::Action* event_action) {
    event_actions_.push_back(event_action);
  }

 private:
  std::vector<user::Action*> event_actions_;

};  // EventAction

}  // namespace g4fire::g4user

#endif  // G4FIRE_G4USER_EVENTACTION_H
