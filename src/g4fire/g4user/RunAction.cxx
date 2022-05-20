#include "RunAction.h"

#include <G4Run.hh>

namespace g4fire::g4user {

void RunAction::BeginOfRunAction(const G4Run* run) {
  // Call user run action
  for (auto& run_action : run_actions_) run_action->BeginOfRunAction(run);
}

void RunAction::EndOfRunAction(const G4Run* run) {
  // Call user run action
  for (auto& run_action : run_actions_) run_action->EndOfRunAction(run);
}

}  // namespace g4fire
