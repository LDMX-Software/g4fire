#ifndef G4FIRE_USERTRACKINFORMATION_H
#define G4FIRE_USERTRACKINFORMATION_H

#include "G4ThreeVector.hh"
#include "G4VUserTrackInformation.hh"
#include "G4Track.hh"

namespace g4fire {

/**
 * Provides user defined information to associate with a Geant4 track.
 *
 * This is helpful for keeping track of information we care about
 * that Geant4 doesn't persist by default.
 */
class UserTrackInformation : public G4VUserTrackInformation {
 public:
  /// Constructor
  UserTrackInformation() = default;

  /**
   * get
   *
   * A static helper function for getting the track information
   * from the passed G4Track. If the track doesn't have an 
   * information attached, a new one is created.
   *
   * @note The return value of this pointer is never NULL.
   *
   * @param track G4Track to get information from
   */
  static UserTrackInformation* get(const G4Track* track);

  /**
   * Initialize the track information with the passed track.
   *
   * We assume the passed track is newly created
   * so we can copy its "current" kinematics and define
   * those kinematics to be the "vertex" kinematics.
   *
   * Even though we are "initializing" the track,
   * we only change the kinematic values. The boolean
   * flags may have been edited prior to the track reaching
   * its own processing phase (where it is initialized),
   * so those flags should (and are) not changed here.
   */
  void initialize(const G4Track* track);

  /// Print the information associated with the track.
  void Print() const final override;

  /**
   * Get the flag which indicates whether this track should be saved
   * as a Trajectory.
   *
   * @return The save flag.
   */
  bool getSaveFlag() const { return save_flag_; }

  /**
   * Set the save flag so the associated track will be persisted
   * as a Trajectory.
   *
   * @param save_flag True to save the associated track.
   */
  void setSaveFlag(bool save_flag) { save_flag_ = save_flag; }

  /**
   * Check whether this track is a brem candidate.
   *
   * @return True if this track is a brem candidate, false otherwise.
   */
  bool isBremCandidate() const { return is_brem_candidate_; }

  /**
   * Tag this track as a brem candidate by the biasing filters.
   *
   * @param is_brem_candidate flag indicating whether this track is
   *      a candidate or not.
   */
  void tagBremCandidate(bool is_brem_candidate = true) {
    is_brem_candidate_ = is_brem_candidate;
  }

  /**
   * Check whether this track is a photon that has undergone a
   * photo-nuclear reaction.
   *
   * @return True if this track is a photon that has undergone a
   * photo-nuclear reaction, false otherwise.
   */
  bool isPNGamma() const { return is_pn_gamma_; }

  /**
   * Tag this track as a photon that has undergone a photo-nuclear
   * reaction.
   *
   * @param is_pn_gamma flag indicating whether this track has
   *      undergone a photo-nuclear reaction or not.
   */
  void tagPNGamma(bool is_pn_gamma = true) { is_pn_gamma_ = is_pn_gamma; }

  /**
   * Get the initial momentum 3-vector of the track [MeV].
   *
   * @return The initial momentum of the track.
   */
  const G4ThreeVector& getInitialMomentum() const { return initial_momentum_; }

  /**
   * Get the name of the volume that this track was created in.
   */
  std::string getVertexVolume() const { return vertex_volume_; }

  /**
   * Get the global time at which this track was created.
   */
  double getVertexTime() const { return vertex_time_; }

 private:
  /**
   * Flag for saving the track as a Trajectory.
   *
   * Default value is false because we want to save space
   * in the output file. We assume everywhere else that
   * the save flag is false unless some other part changes it.
   */
  bool save_flag_{false};

  /// Flag indicating whether this track is a brem candidate
  bool is_brem_candidate_{false};

  /**
   * Flag indicating whether this track has undergone a photo-nuclear
   * reaction.
   */
  bool is_pn_gamma_{false};

  /// Volume the track was created in.
  std::string vertex_volume_{""};

  /// Global Time of Creation
  double vertex_time_{0.};

  /// The initial momentum of the track.
  G4ThreeVector initial_momentum_;
};
}  // namespace g4fire

#endif
