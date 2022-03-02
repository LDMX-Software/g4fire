
#include "g4fire/UserAction.h"

/*~~~~~~~~~~~~~*/
/*   g4fire   */
/*~~~~~~~~~~~~~*/
#include "g4fire/PluginFactory.h"

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4Track.hh"

namespace g4fire {

UserAction::UserAction(const std::string& name,
                       framework::config::Parameters& parameters) {
  name_ = name;
  parameters_ = parameters;
}

UserAction::~UserAction() {}

void UserAction::declare(const std::string& className,
                         UserActionBuilder* builder) {
  PluginFactory::getInstance().registerAction(className, builder);
}
}  // namespace g4fire
