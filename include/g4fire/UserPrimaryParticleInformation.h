#ifndef G4FIRE_USERPRIMARYPARTICLEINFORMATION_H_
#define G4FIRE_USERPRIMARYPARTICLEINFORMATION_H_

#include "G4VUserPrimaryParticleInformation.hh"

namespace g4fire {

/**
 * @brief Defines extra information attached to a Geant4 primary particle
 */
class UserPrimaryParticleInformation
    : public G4VUserPrimaryParticleInformation {
 public:
  /// Class Constructor.
  UserPrimaryParticleInformation() { ; }

  /// Class destructor.
  virtual ~UserPrimaryParticleInformation() { ; }

  /**
   * Set the HEP event status (generator status) e.g. from an LHE particle.
   * @param hep_event_status The HEP event status.
   */
  void setHepEvtStatus(int hep_event_status) { hep_event_status_ = hep_event_status; }

  /**
   * Get the HEP event status.
   * @return The HEP event status.
   */
  int getHepEvtStatus() { return hep_event_status_; }

  /**
   * Implement virtual method (no-op).
   */
  void Print() const {}

 private:
  /**
   * The HEP event status.
   */
  int hep_event_status_{-1};
};

}  // namespace g4fire

#endif
