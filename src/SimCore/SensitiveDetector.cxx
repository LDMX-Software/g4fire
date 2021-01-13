#include "SimCore/SensitiveDetector.h"

#include "G4SDManager.hh"

#include "Framework/Exception/Exception.h"
#include "SimCore/PluginFactory.h"

namespace simcore {

SensitiveDetector::SensitiveDetector(
    const std::string& name, simcore::ConditionsInterface& ci,
    const framework::config::Parameters& parameters)
    : G4VSensitiveDetector(name), conditions_interface_(ci) {
  // register us with the manager
  G4SDManager::GetSDMpointer()->AddNewDetector(this);
}

SensitiveDetector::~SensitiveDetector() {}

void SensitiveDetector::declare(const std::string& className,
                                SensitiveDetectorBuilder* builder) {
  PluginFactory::getInstance().registerSensitiveDetector(className, builder);
}

}  // namespace simcore
