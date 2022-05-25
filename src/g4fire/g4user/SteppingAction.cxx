#include "SteppingAction.h"

namespace g4fire::g4user {

void SteppingAction::UserSteppingAction(const G4Step *step) {
  for (auto &stepping_action : stepping_actions_)
    stepping_action->stepping(step);
}
} // namespace g4fire
