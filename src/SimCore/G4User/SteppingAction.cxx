/**
 * @file USteppingAction.cxx
 * @author Omar Moreno, SLAC National Accelerator Laboraty
 */

#include "SimCore/G4User/SteppingAction.h"

namespace simcore {
namespace g4user {

void SteppingAction::UserSteppingAction(const G4Step* step) {
  for (auto& steppingAction : steppingActions_) steppingAction->stepping(step);
}

}  // namespace g4user
}  // namespace simcore
