#pragma once

#include <vector>

#include "fire/io/Data.h"

namespace g4fire::event {

/**
 * @brief Represents a simulated tracker hit in the simulation
 */
class SimTrackerHit {
public:
  /// Constructor
  SimTrackerHit() = default;

  /// Destructor
  ~SimTrackerHit() = default;

  /// Reset an instance of this class by clearing all of its data.
  void clear();

  /// @return The detector ID of the hit.
  int id() const { return id_; };

  /// @return The geometric layer ID of the hit.
  short layerID() const { return layer_id_; };

  /**
   * Get the module ID associated with a hit.  This is used to
   * uniquely identify a sensor within a layer.
   *
   * @return The module ID associated with a hit.
   */
  short moduleID() const { return module_id_; };

  /**
   * Get the (x, y, z) position of the hit in mm.
   *
   * @return A vector representation of the x, y, z position of a hit.
   */
  std::vector<float> position() const { return {x_, y_, z_}; };

  /// @return The energy deposited on the hit in MeV.
  float edep() const { return edep_; };

  /// @return The energy of the hit in MeV.
  float energy() const { return energy_; };

  /// @return The global time of the hit in ns.
  float time() const { return time_; };

  /**
   * Get the path length between the start and end points of the
   * hit [mm].
   *
   * @return The path length of the hit.
   */
  float pathLength() const { return path_length_; };

  /**
   * Get the momentum of the particle at the position at which
   * the hit took place [MeV].
   *
   * @return A vector representation of the momentum of the particle.
   */
  std::vector<float> momentum() const { return {px_, py_, pz_}; };

  /// @return The Geant4 track ID of the particle associated with this hit.
  short trackID() const { return track_id_; };

  /// @return The PDG ID of the SimParticle associated with this hit.
  int pdgID() const { return pdg_id_; };

  /**
   * Set the detector ID of the hit.
   *
   * @param[in] id The detector ID of the hit.
   */
  void setID(const long id) { id_ = id; };

  /**
   * Set the geometric layer ID of the hit.
   *
   * @param[in] layer_id The layer ID of the hit.
   */
  void setLayerID(const int layer_id) { layer_id_ = layer_id; };

  /**
   * Set the module ID associated with a hit.  This is used to
   * uniquely identify a sensor within a layer.
   *
   * @param[in] module_id The module ID associated with a hit.
   */
  void setModuleID(const int module_id) { module_id_ = module_id; };

  /**
   * Set the position of the hit in mm.
   *
   * @param[in] x The x position.
   * @param[in] y The y position.
   * @param[in] z The z position.
   */
  inline void setPosition(const float x, const float y, const float z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  /**
   * Set the energy deposited on the hit in MeV.
   *
   * @param[in] edep The energy deposited on the hit.
   */
  void setEdep(const float edep) { edep_ = edep; };

  /**
   * Set the energy of the hit.
   *
   * @param[in] energy The energy of the hit.
   */
  void setEnergy(const float energy) { energy_ = energy; };

  /**
   * Set the global time of the hit in ns.
   *
   * @param[in] time The global time of the hit.
   */
  void setTime(const float time) { time_ = time; };

  /**
   * Set the path length of the hit in mm.
   *
   * @param[in] path_length The path length of the hit.
   */
  void setPathLength(const float path_length) { path_length_ = path_length; };

  /**
   * Set the momentum of the particle at the position at which
   * the hit took place in MeV.
   *
   * @param[in] px The X momentum.
   * @param[in] py The Y momentum.
   * @param[in] pz The Z momentum.
   */
  inline void setMomentum(const float px, const float py, const float pz) { 
    px_ = px; 
    py_ = py; 
    pz_ = pz; 
  };

  /**
   * Set the Geant4 track ID of the particle associted with this hit.
   *
   * @param[in] track_id The Sim particle track ID of the hit.
   */
  void setTrackID(const int track_id) { track_id_ = track_id; };

  /**
   * Set the PDG ID of the SimParticle associated with this hit.
   *
   * @param[in] pdg_id The PDG ID of the SimParticle associated with this hit.
   */
  void setPdgID(const int pdg_id) { pdg_id_ = pdg_id; };

  /// Sort by time of hit
  bool operator<(const g4fire::event::SimTrackerHit &rhs) const {
    return time() < rhs.time();
  }
  
  /**
   * Overload the stream insertion operator to output a string representation
   * of this SimTrackerHit.
   *
   * @param[in] output The output stream where the string representation will
   *    be inserted.
   * @param[in] particle The SimTrackerHit to stringify.
   *
   * @return[out] An ostream object with the string representation of
   *    SimTrackerHit inserted.
   */
  friend std::ostream &operator<<(std::ostream &output,
                                  const SimTrackerHit &hit);

private:
  /// The detector ID.
  int id_{0};

  /// The layer ID.
  int layer_id_{0};

  /// The module ID.
  int module_id_{0};

  /// The energy deposited on the hit.
  float edep_{0};

  /// The global time of the hit.
  float time_{0};

  /// The x component of the momentum.
  float px_{0};

  /// The y component of the momentum.
  float py_{0};

  /// The z component of the momentum.
  float pz_{0};

  /// The hit energy.
  float energy_{0};

  /// The x position.
  float x_{0};

  /// The y position.
  float y_{0};

  /// The z position.
  float z_{0};

  /// The path length of the hit.
  float path_length_{0};

  /// The Geant4 track ID.
  int track_id_{0};

  /// The PDG ID of the particle associated with this track.
  int pdg_id_{0};

  friend class fire::io::Data<SimTrackerHit>;
  void attach(fire::io::Data<SimTrackerHit> &d);

}; // SimTrackerHit
} // namespace g4fire::event
