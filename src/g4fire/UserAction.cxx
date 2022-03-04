#include "g4fire/UserAction.h"

#include "g4fire/PluginFactory.h"

#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4Track.hh"

namespace g4fire {

UserAction::UserAction(const std::string& name,
                       fire::config::Parameters& params) {
  name_ = name;
  params_ = params;
}

void UserAction::declare(const std::string& class_name,
                         UserActionBuilder* builder) {
  PluginFactory::getInstance().registerAction(class_name, builder);
}
}  // namespace g4fire
