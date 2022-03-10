#pragma once

#include "G4GeneralParticleSource.hh"

#include "g4fire/PrimaryGenerator.h"

#include "fire/config/Parameters.h" 

class G4Event;

namespace g4fire {

/**
 * @brief Class that extends the functionality of G4GeneralParticleSource.
 */
class GeneralParticleSource : public PrimaryGenerator {
 public:
  /**
   * Constructor.
   *
   * @param params Parameters used to configure the particle gun.
   *
   * Parameters:
   *  initCommands : vector of Geant4 strings to initialize the GPS
   */
  GeneralParticleSource(const std::string& name,
                        fire::config::Parameters& params);

  /// Destructor
  ~GeneralParticleSource();

  /**
   * Generate the primary vertices in the Geant4 event.
   *
   * @param event The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

 private:
  /**
   * The underlying Geant4 GPS implementation.
   *
   * The creation of this class creates a new messenger that we can pass
   * commands to.
   */
  G4GeneralParticleSource g4_source_;

};  // GeneralParticleSource

}  // namespace g4fire
