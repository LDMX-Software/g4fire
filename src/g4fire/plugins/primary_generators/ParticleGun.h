
#include "G4ParticleGun.hh"

#include "g4fire/PrimaryGenerator.h"

#include "fire/config/Parameters.h" 

class G4Event;

namespace g4fire {

/**
 * @brief Class that extends the functionality of G4ParticleGun.
 */
class ParticleGun : public PrimaryGenerator {
 public:
  /**
   * Constructor.
   *
   * @param params Parameters used to configure the particle gun.
   *
   * Parameters:
   *  verbosity: > 1 means print configuration
   *  particle : name of particle to shoot (Geant4 naming)
   *  energy   : energy of particle (GeV)
   *  position : position to shoot from (mm three-vector)
   *  time     : time to shoot at (ns)
   *  direction: direction to shoot in (unitless three-vector)
   */
  ParticleGun(const std::string& name,
              fire::config::Parameters& params);

  /// Destructor
  ~ParticleGun() = default;

  /**
   * Generate the primary vertices in the Geant4 event.
   *
   * @param event The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

 private:
  /**
   * The actual Geant4 implementation of the ParticleGun
   */
  G4ParticleGun gun_;

  /**
   * LDMX Verbosity for this generator
   */
  int verbosity_;

};  // ParticleGun

}  // namespace g4fire
