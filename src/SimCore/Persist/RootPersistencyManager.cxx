
#include "SimCore/Persist/RootPersistencyManager.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <algorithm>
#include <memory>

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/EventHeader.h"
#include "Framework/RunHeader.h"
#include "Framework/Version.h"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/DetectorConstruction.h"
#include "SimCore/PluginFactory.h"
#include "SimCore/Event/SimTrackerHit.h"
#include "SimCore/RunManager.h"
#include "SimCore/UserEventInformation.h"
#include "SimCore/Event/SimTrackerHit.h"
#include "SimCore/Event/SimCalorimeterHit.h"
#include "SimCore/G4User/TrackingAction.h"

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4RunManagerKernel.hh"

namespace simcore {
namespace persist {

RootPersistencyManager::RootPersistencyManager(framework::EventFile &file,
                                               framework::config::Parameters &parameters,
                                               const int &runNumber)
    : G4PersistencyManager(G4PersistencyCenter::GetPersistencyCenter(),
                           "RootPersistencyManager"),
      file_(file) {
  // Let Geant4 know what to use this persistency manager
  G4PersistencyCenter::GetPersistencyCenter()->RegisterPersistencyManager(this);
  G4PersistencyCenter::GetPersistencyCenter()->SetPersistencyManager(
      this, "RootPersistencyManager");

  // Set the parameters, used laster when printing run header
  parameters_ = parameters;

  run_ = runNumber;
}

G4bool RootPersistencyManager::Store(const G4Event *anEvent) {
  // Check if the event has been aborted.  If so, skip storage of the
  // event.
  if (G4RunManager::GetRunManager()->GetCurrentEvent()->IsAborted())
    return false;

  // Set basic event information.
  writeHeader(anEvent);

  // Set pointer to current G4Event.
  simParticleBuilder_.setCurrentEvent(anEvent);

  // Build the SimParticle list for the output ROOT event.
  simParticleBuilder_.buildSimParticles(event_);

  // Copy hit objects from SD hit collections into the output event.
  for (auto& sd : simcore::PluginFactory::getInstance().getSensitiveDetectors()) {
    sd->saveHits(*event_);
  }

  return true;
}

G4bool RootPersistencyManager::Store(const G4Run *) {
  // NOTE: This method is called once the run is terminated through
  // the run manager.

  // throws an exception if not correct run number
  auto runHeader = file_.getRunHeader(run_);

  // Set parameter value with number of events processed.
  runHeader.setIntParameter("Event Count", eventsCompleted_);
  runHeader.setIntParameter("Events Began", eventsBegan_);

  return true;
}

void RootPersistencyManager::Initialize() {}

void RootPersistencyManager::writeHeader(const G4Event *anEvent) {
  // Retrieve a mutable version of the event header
  framework::EventHeader &eventHeader = event_->getEventHeader();

  // Set the event weight
  double weight{1};
  if (anEvent->GetUserInformation() != nullptr) {
    weight = static_cast<UserEventInformation *>(anEvent->GetUserInformation())
                 ->getWeight();
  } else if (anEvent->GetPrimaryVertex(0)) {
    weight = anEvent->GetPrimaryVertex(0)->GetWeight();
  }
  eventHeader.setWeight(weight);

  // Save the state of the random engine to an output stream. A string
  // is then extracted and saved to the event header.
  std::ostringstream stream;
  G4Random::saveFullState(stream);
  // std::cout << stream.str() << std::endl;
  eventHeader.setStringParameter("eventSeed", stream.str());
}

}  // namespace persist
}  // namespace simcore
