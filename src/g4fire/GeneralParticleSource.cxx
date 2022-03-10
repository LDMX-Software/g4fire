#include "g4fire/GeneralParticleSource.h"

#include "G4Event.hh"
#include "G4UImanager.hh"

#include "fire/exception/Exception.h"

namespace g4fire {

GeneralParticleSource::GeneralParticleSource(
    const std::string& name, fire::config::Parameters& params)
    : PrimaryGenerator(name, params) {
  auto initCommands{
      params_.get<std::vector<std::string> >("initCommands")};

  for (const auto& cmd : initCommands) {
    int g4Ret = G4UImanager::GetUIpointer()->ApplyCommand(cmd);
    if (g4Ret > 0) {
      throw fire::Exception("InitCmd",
                      "Initialization command '" + cmd +
                          "' returned a failue status from Geant4: " +
                          std::to_string(g4Ret), false);
    }
  }
}

GeneralParticleSource::~GeneralParticleSource() {}

void GeneralParticleSource::GeneratePrimaryVertex(G4Event* event) {
  // just pass to the Geant4 implementation
  g4_source_.GeneratePrimaryVertex(event);

  return;
}

}  // namespace g4fire

DECLARE_GENERATOR(g4fire, GeneralParticleSource)
