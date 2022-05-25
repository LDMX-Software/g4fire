#include "PrimaryGeneratorAction.h"

#include <G4Event.hh>
#include <G4RunManager.hh>  // Needed for CLHEP

#include <fire/exception/Exception.h> 

#include "g4fire/TransientInformation.h"

namespace g4fire::g4user {

PrimaryGeneratorAction::PrimaryGeneratorAction(std::vector<std::unique_ptr<user::PrimaryGenerator>>& pgs)
    : G4VUserPrimaryGeneratorAction() {
  for (auto& pg : pgs) generators_.push_back(pg.get());
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
  event->SetUserInformation(new EventInformation);

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
