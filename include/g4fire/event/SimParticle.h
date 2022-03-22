#pragma one

#include <map>
#include <string>
#include <vector>

#include "fire/io/Data.h"

namespace g4fire::event {

/**
 * Class representing a simulated particle.
 *
 * The particles are created from the tracks produced by simulation.
 */
class SimParticle {
 public:
  /**
   * Enum for interesting process types.  The names of these process
   * types are mapped directly to their Geant4 counterpart.
   */
  enum ProcessType {
    unknown = 0,
    annihil,
    compt,
    conv,
    electronNuclear,
    eBrem,
    eIoni,
    msc,
    phot,
    photonNuclear,
    GammaToMuPair,
    eDarkBrem,
    // Only add additional processes to the end of this list!
  };

  /// Typedef for process map.
  typedef std::map<std::string, ProcessType> ProcessTypeMap;

  /// Constructor
  SimParticle() = default;

  /// Destructor
  ~SimParticle() = default;

  /// Reset an instance of this class by clearing all of its data.
  void clear();

  /// Print a string representation of this object.
  // void Print() const;

  /**
   * Get the energy of this particle [MeV].
   *
   * @return The energy of this particle.
   */
  double getEnergy() const { return energy_; }

  /**
   * Get the PDG ID of this particle.
   *
   * @return The PDG ID of this particle.
   */
  int getPdgID() const { return pdg_id_; }

  /**
   * Get the generator status of this particle.  A non-zero status
   * indicates that this particle originates from an external
   * generator (e.g. LHE).
   *
   * @return The generator status.
   */
  int getGenStatus() const { return gen_status_; }

  /**
   * Get the global time of this particle's creation [ns].
   *
   * @return The global time of this particle's creation.
   */
  double getTime() const { return time_; }

  /**
   * Get a vector containing the vertex of this particle in mm.
   *
   * In this case, the vertex refers to the position where the
   * particle is first created in the simulation.   For a particle
   * with generator status equal to 1, this will equal the position
   * from which this particle is fired from.
   *
   * @return The vertex of this particle.
   */
  std::vector<double> getVertex() const { return {x_, y_, z_}; }

  /**
   * Get the volume name in which this particle was created in.
   *
   * The volumes names are set in the GDML detector description.
   *
   * @return The volume name in which this particle was created in.
   */
  std::string getVertexVolume() const { return vertex_vol_; }

  /**
   * Get the endpoint of this particle where it was destroyed
   * or left the world volume [mm].
   *
   * @return The endpoint of this particle
   */
  std::vector<double> getEndPoint() const { return {endx_, endy_, endz_}; }

  /**
   * Get a vector containing the momentum of this particle [MeV].
   *
   * The momentum of this particle is set at the time of its creation.
   *
   * @return The momentum of this particle.
   */
  std::vector<double> getMomentum() const { return {px_, py_, pz_}; }

  /**
   * Get the mass of this particle [GeV].
   *
   * @return The mass of this particle in GeV.
   */
  double getMass() const { return mass_; }

  /**
   * Get the charge of this particle.
   *
   * @return The charge of this particle.
   */
  double getCharge() const { return charge_; }

  /**
   * Get a vector containing the track IDs of all daughter particles.
   *
   * @return A vector containing the track IDs of all daughter
   *      particles.
   */
  std::vector<int> getDaughters() const { return daughters_; }

  /**
   * Get a vector containing the track IDs of the parent particles.
   *
   * @return A vector containing the track IDs the parent particles.
   */
  std::vector<int> getParents() const { return parents_; }

  /**
   * Set the energy of this particle [MeV].
   *
   * @param[in] energy the energy of this particle.
   */
  void setEnergy(const double &energy) { energy_ = energy; }

  /**
   * Set the PDG ID of this particle.
   *
   * @param[in] pdg_id the PDG ID of the hit.
   */
  void setPdgID(const int &pdg_id) { pdg_id_ = pdg_id; }

  /**
   * Set the generator status of this particle.
   *
   * @param[in] gen_status the generator status of the hit.
   */
  void setGenStatus(const int &gen_status) { gen_status_ = gen_status; }

  /**
   * Set the global time of this particle's creation [ns].
   *
   * @param[in] time The global time of this particle's creation.
   */
  void setTime(const double &time) { time_ = time; }

  /**
   * Set the vertex of this particle [mm].
   *
   * @param[in] x The vertex x position.
   * @param[in] y The vertex y position.
   * @param[in] z The vertex z position.
   */
  void setVertex(const double &x, const double &y, const double &z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  /**
   * Set the name of the volume that this particle was created in.
   *
   * @param[in] vertex_vol volume name that this particle was
   *      created in.
   */
  void setVertexVolume(const std::string &vertex_vol) {
    vertex_vol_ = vertex_vol;
  }

  /**
   * Set the end point position of this particle [mm].
   *
   * @param[in] endx The x position of the end point.
   * @param[in] endy The y position of the end point.
   * @param[in] endz The z position of the end point.
   */
  void setEndPoint(const double &endx, const double &endy, const double &endz) {
    endx_ = endx;
    endy_ = endy;
    endz_ = endz;
  }

  /**
   * Set the momentum of this particle [MeV].
   *
   * @param[in] px The x momentum component.
   * @param[in] py The y momentum component.
   * @param[in] pz The z momentum component.
   */
  void setMomentum(const double &px, const double &py, const double &pz) {
    px_ = px;
    py_ = py;
    pz_ = pz;
  }

  /**
   * Set the mass of this particle [GeV].
   *
   * @param[in] mass The mass of this particle.
   */
  void setMass(const double &mass) { mass_ = mass; }

  /**
   * Set the charge of this particle.
   *
   * @param[in] charge The charge of this particle.
   */
  void setCharge(const double &charge) { charge_ = charge; }

  /**
   * Add a reference to a daughter particle by its track ID.
   *
   * This adds the track ID of the daughter particle to the vector of
   * daughter particle IDs.
   *
   * @param[in] daughterTrackID The daughter particle track ID.
   */
  void addDaughter(const int &daughterTrackID) {
    daughters_.push_back(daughterTrackID);
  }

  /**
   * Add a reference to a parent particle by its track ID.
   *
   * @param[in] parentTrackID The track ID of the parent particle.
   */
  void addParent(const int &parentTrackID) {
    parents_.push_back(parentTrackID);
  }

  /**
   * Get the creator process type of this particle.
   *
   * @return The creator process type of this particle.
   */
  int getProcessType() const { return process_type_; }

  /**
   * Set the creator process type of this particle.
   *
   * @param[in] process_type the creator process type of this particle.
   */
  void setProcessType(const int &process_type) { process_type_ = process_type; }

  /**
   * Set the momentum at this particle's end point.
   *
   * @param[in] endpx The x component of the momentum.
   * @param[in] endpy The y component of the momentum.
   * @param[in] endpz The z component of the momentum.
   */
  void setEndPointMomentum(const double &endpx, const double &endpy,
                           const double &endpz) {
    endpx_ = endpx;
    endpy_ = endpy;
    endpz_ = endpz;
  }

  /**
   * Get the momentum at this particle's end point.
   *
   * @return The momentum at this particle's end point as a vector.
   */
  std::vector<double> getEndPointMomentum() const {
    return {endpx_, endpy_, endpz_};
  }

  /**
   * Get the process type enum from a G4VProcess name.
   *
   * @return The process type from the string.
   */
  static ProcessType findProcessType(std::string processName);

 private:
  static ProcessTypeMap createProcessTypeMap();

  /// The energy of this particle.
  double energy_{0};

  /// The PDG ID of this particle.
  int pdg_id_{0};

  /// The generator status.
  int gen_status_{-1};

  /// The global creation time.
  double time_{0};

  /// The x component of the vertex.
  double x_{0};

  /// The y component of the vertex.
  double y_{0};

  /// The z component of the vertex.
  double z_{0};

  /// The x component of the end point.
  double endx_{0};

  /// The y component of the end point.
  double endy_{0};

  /// The z component of the end point.
  double endz_{0};

  /// The x component of the momentum.
  double px_{0};

  /// The y component of the momentum.
  double py_{0};

  /// The z component of the momentum.
  double pz_{0};

  /// The x component of the endpoint momentum.
  double endpx_{0};

  /// The y component of the endpoint momentum.
  double endpy_{0};

  /// The z component of the endpoint momentum.
  double endpz_{0};

  /// The particle's mass.
  double mass_{0};

  /// The particle's charge.
  double charge_{0};

  /// The list of daughter particle track IDs.
  std::vector<int> daughters_;

  /// The list of parent particles track IDs.
  std::vector<int> parents_;

  /// Encoding of Geant4 process type.
  int process_type_{-1};

  /// Volume the track was created in.
  std::string vertex_vol_{""};

  /// Map containing the process types.
  static ProcessTypeMap PROCESS_MAP;

  friend class fire::io::Data<SimParticle>;
  void attach(fire::io::Data<SimParticle> &d);

}; // SimParticle
} // namespace g4fire::event
