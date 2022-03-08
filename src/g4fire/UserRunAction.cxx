#include "g4fire/UserRunAction.h"

#include "G4Run.hh"

namespace g4fire {

void UserRunAction::BeginOfRunAction(const G4Run* run) {
  // Call user run action
  for (auto& run_action : run_actions_) run_action->BeginOfRunAction(run);
}

void UserRunAction::EndOfRunAction(const G4Run* run) {
  // Call user run action
  for (auto& run_action : run_actions_) run_action->EndOfRunAction(run);
}

}  // namespace g4fire
