#pragma once

#include <string>
#include <vector>

#include "g4fire/event/SimParticle.h"

#include "fire/io/Data.h"

namespace g4fire::event {

/**
 * @brief Stores simulated calorimeter hit information
 *
 * @note
 * This class represents simulated hit information from a calorimeter detector.
 * It provides access to the cell ID, energy deposition, cell position and time.
 * Additionally, individual depositions or steps from MC particles are tabulated
 * as contributions stored in vectors.  Contribution information includes a
 * reference to the relevant SimParticle, the PDG code of the actual particle
 * which deposited energy (may be different from the actual SimParticle), the
 * time of the contribution and the energy deposition.
 */
class SimCalorimeterHit {
 public:
  /**
   * @brief Information about a contribution to the hit in the associated cell
   */
  struct Contrib {
    /**
     * track_id of incident particle that is an ancestor of the contributor
     *
     * The incident ancestor is found in TrackMap::findIncident where the
     * ancestry is looped upwards until a particle is found that matches
     * the criteria.
     *      (1) particle will be saved to output file AND
     *      (2) particle originates in a region outside the CalorimeterRegion
     * If no particle is found matching these criteria, the primary particle
     * that is this track_id's ancestor is chosen.
     */
    int incident_id{-1};

    /// track ID of this contributor
    int track_id{-1};

    /// PDG ID of this contributor
    int pdg_id{0};

    /// Energy depostied by this contributor
    float edep{0};

    /// Time this contributor made the hit (global Geant4 time)
    float time{0};
  };

  /// Constructor
  SimCalorimeterHit() = default;

  /// Destructor
  ~SimCalorimeterHit();

  /// Reset an instance of this class by clearing all of its data.
  void clear();

  /// @return The detector ID.
  int id() const { return id_; }

  /**
   * Set the detector ID.
   *
   * @param[in] id The detector ID.
   */
  void setID(const int id) { id_ = id; }

  /// @return The energy deposition of the hit in MeV.
  float edep() const { return edep_; }

  /**
   * Set the energy deposition of the hit [MeV].
   *
   * @param[in] edep The energy deposition of the hit.
   */
  void setEdep(const float edep) { edep_ = edep; }

  /// @return A vector containing the x, y, and z positions of the hit in mm.
  std::vector<float> position() const { return {x_, y_, z_}; }

  /**
   * Set the XYZ position of the hit in mm.
   *
   * @param[in] x The x position.
   * @param[in] y The y position.
   * @param[in] z The z position.
   */
  void setPosition(const float x, const float y, const float z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  /// @return The global time of the hit in ns.
  float time() const { return time_; }

  /**
   * Set the time of the hit [ns].
   *
   * @param[in] time The time of the hit.
   */
  void setTime(const float time) { time_ = time; }

  /// @return The number of hit contributions.
  unsigned contribCount() const { return contrib_count_; }

  /**
   * Add a hit contribution from a SimParticle.
   *
   * @param[in] incident_id The Geant4 track ID for the particle's parent
   * incident on the calorimeter region
   * @param[in] track_id The Geant4 track ID for the particle
   * @param[in] pdg_id The PDG code of the actual track.
   * @param[in] edep The energy deposition of the hit [MeV].
   * @param[in] time The time of the hit [ns].
   */
  void addContrib(const int &incident_id, const int &track_id,
                  const int &pdg_id, const float &edep, const float &time);

  /**
   * Get a hit contribution by index.
   *
   * @param[i] i The index of the hit contribution.
   * @return The hit contribution at the index.
   */
  Contrib getContrib(const int &i) const;

  /**
   * Find the index of a hit contribution from a SimParticle and PDG code.
   *
   * @param[in] track_id The track ID of the particle causing the hit
   * @param[in] pdg_id The PDG code of the contribution.
   * @return The index of the contribution or -1 if none exists.
   */
  int findContribIndex(const int &track_id, const int &pdg_id) const;

  /**
   * Update an existing hit contribution by incrementing its edep and setting
   * the time if the new time is less than the old one.
   *
   * @param[in] i The index of the contribution.
   * @param[in] edep The additional energy contribution [MeV].
   * @param[in] time The time of the contribution [ns].
   */
  void updateContrib(const int &i, const float &edep, const float &time);

  /// Sort by time of hit
  bool operator<(const SimCalorimeterHit &rhs) const {
    return time() < rhs.time();
  }

  /**
   * Overload the stream insertion operator to output a string representation
   * of this SimCalorimeterHit.
   *
   * @param[in] output The output stream where the string representation will
   *    be inserted.
   * @param[in] particle The SimCalorimeterHit to stringify.
   *
   * @return[out] An ostream object with the string representation of
   *    SimCalorimeterHit inserted.
   */
  friend std::ostream &operator<<(std::ostream &output,
                                  const SimCalorimeterHit &hit);

 private:
  /// The detector ID.
  int id_{0};

  /// The energy deposition in MeV.
  float edep_{0};

  /// The X position in mm.
  float x_{0};

  /// The Y position in mm.
  float y_{0};

  /// The Z position in mm.
  float z_{0};

  /// The global time of the hit in ns.
  float time_{0};

  /// The list of track IDs contributing to the hit.
  std::vector<int> contribs_track_id_;

  /// The list of incident IDs contributing to the hit
  std::vector<int> contribs_incident_id_;

  /// The list of PDG codes contributing to the hit.
  std::vector<int> contribs_pdg_id_;

  /// The list of energy depositions contributing to the hit.
  std::vector<float> contribs_edeps_;

  /// The list of times contributing to the hit.
  std::vector<float> contribs_time_;

  /// The number of hit contributions.
  unsigned contrib_count_{0};

  friend class fire::io::Data<SimCalorimeterHit>;
  void attach(fire::io::Data<SimCalorimeterHit> &d);
}; // SimCalorimeterHit
} // namespace g4fire::event
