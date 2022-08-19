#include "g4fire/user/SensitiveDetector.h"

#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4Step.hh"

namespace g4fire::user {
SensitiveDetector::SensitiveDetector(g4fire::ConditionsInterface& ci,
                  const fire::config::Parameters& p)
  : G4VSensitiveDetector(p.get<std::string>("name")),
    conditions_interface_{ci} {}

bool SensitiveDetector::isGeantino(const G4Step* step) const {
  auto particle_def{step->GetTrack()->GetDefinition()};
  return (particle_def == G4Geantino::Definition() or
          particle_def == G4ChargedGeantino::Definition());

}

}
