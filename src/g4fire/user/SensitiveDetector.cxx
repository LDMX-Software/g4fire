#include "g4fire/user/SensitiveDetector.h"

namespace g4fire::user {
SensitiveDetector::SensitiveDetector(g4fire::ConditionsInterface& ci,
                  const fire::config::Parameters& p)
  : G4VSensitiveDetector(p.get<std::string>("name")),
    conditions_interface_{ci} {}
}
