/**
 * @file UserRunAction.cxx
 * @brief Class which implements user run action
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#include "g4fire/UserRunAction.h"

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4Run.hh"

namespace g4fire {

UserRunAction::UserRunAction() {}

UserRunAction::~UserRunAction() {}

void UserRunAction::BeginOfRunAction(const G4Run* run) {
  // Call user run action
  for (auto& runAction : runActions_) runAction->BeginOfRunAction(run);
}

void UserRunAction::EndOfRunAction(const G4Run* run) {
  // Call user run action
  for (auto& runAction : runActions_) runAction->EndOfRunAction(run);
}

}  // namespace g4fire
