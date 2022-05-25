#include "PrimaryGeneratorAction.h"

#include <G4Event.hh>
#include <G4RunManager.hh>  // Needed for CLHEP

#include <fire/exception/Exception.h> 

#include "g4fire/TransientInformation.h"

namespace g4fire::g4user {

PrimaryGeneratorAction::PrimaryGeneratorAction(const fire::config::Parameters& params)
    : G4VUserPrimaryGeneratorAction() {
  auto generators{
      params.get<std::vector<fire::config::Parameters> >(
          "generators", {})};
  if (generators.empty()) {
    throw fire::Exception("MissingGenerator",
                    "Need to define some generator of primaries.", false);
  }

  for (auto& generator : generators) {
    generators_.emplace_back(
        user::PrimaryGenerator::Factory::make(generator.get<std::string>("class_name"), generator)
        );
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  /*
   * Create our Event information first so that it
   * can be accessed by everyone from now on.
   */
  // Make sure we aren't overwriting a different information container
  if (event->GetUserInformation()) {
    throw fire::Exception(
        "Misconfig",
        "There was a UserEventInformation attached before beginning event."
        "\nI don't know how this happend!!", false);
  }

  // Make our information container and give it to geant4
  //    G4Event owns the event information and will delete it
  event->SetUserInformation(new UserEventInformation);

  // Generate the primary vertices using the generators
  std::for_each(generators_.begin(), generators_.end(),
                [event](const auto& generator) {
                  generator->GeneratePrimaryVertex(event);
                });

  if (event->GetNumberOfPrimaryVertex() == 0) {
    throw fire::Exception(
        "NoPrimaries",
        "No primary vertices were produced by any of the generators.", false);
  }
}
}  // namespace g4fire
