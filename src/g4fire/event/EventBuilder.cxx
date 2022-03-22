#include "g4fire/event/EventBuilder.h"

#include <string>

#include "G4Event.hh"
#include "G4RunManager.hh"

#include "fire/EventHeader.h"

#include "g4fire/UserTrackingAction.h"

namespace g4fire::event {

void EventBuilder::writeEvent(const G4Event *g4event, fire::Event &event) {
  writeHeader(g4event, event);

  auto tracks{UserTrackingAction::getUserTrackingAction()->getTrackMap()};
  
  tracks->traceAncestry();
  event.add("SimParticles", tracks->particleMap());
}

void EventBuilder::writeHeader(const G4Event *g4event, fire::Event &event) {

  // Retrieve a mutable version of the event header
  fire::EventHeader &header{event.header()};

  // auto event_info{
  //  static_cast<UserEventInformation*>(g4event->GetUserInformation())};

  // Save the state of the random engine to an output stream. A string
  // is then extracted and saved to the event header.
  std::ostringstream stream;
  G4Random::saveFullState(stream);
  // std::cout << stream.str() << std::endl;
  header.set<std::string>("event_seed", stream.str());
}

} // namespace g4fire::event
