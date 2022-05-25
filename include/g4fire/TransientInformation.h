#ifndef G4FIRE_TRANSIENTINFORMATION_H
#define G4FIRE_TRANSIENTINFORMATION_H

#include <fire/config/Parameters.h>

#include <G4VUserEventInformation.hh>
#include <G4VUserRegionInformation.hh>
#include <G4VUserTrackInformation.hh>
#include <G4VUserPrimaryParticleInformation.hh>

#include <G4Event.hh>
#include <G4Track.hh>

namespace g4fire {

/**
 * Geant4 has several different classes that are all
 * ways of storing data about different objects in memory
 * during the simulation of an event.
 *
 * This class encapsulates data that is only used during
 * the simulation of an event. It has the exact same implementation
 * as fire::config::Parameters, but having a different name for
 * it is helpful for developers.
 */
using TransientInformation = ::fire::config::Parameters;

/**
 * The event information is transient information about the entire event
 */
class EventInformation : public G4VUserEventInformation, public TransientInformation {
 public:
  virtual void Print() const final override {}
  static EventInformation* get(const G4Event* e) {
    return dynamic_cast<EventInformation*>(e->GetUserInformation());
  }
};

/**
 * The region information is transient information about regions
 */
class RegionInformation : public G4VUserRegionInformation, public TransientInformation {};

/**
 * The track information is transient information about tracks
 */
class TrackInformation : public G4VUserTrackInformation, public TransientInformation {
 public:
  /**
   * Helper function to get the transient information about a track
   */
  static TrackInformation* get(const G4Track* t) {
    return dynamic_cast<TrackInformation*>(t->GetUserInformation());
  }
};

/**
 * The primary particle information is transient information about primary particles
 */
class PrimaryParticleInformation : public G4VUserPrimaryParticleInformation, 
                                   public TransientInformation {
 public:
  virtual void Print() const final override {}
};

}

#endif
