#pragma once 

#include "G4VPhysicsConstructor.hh"

#include "fire/config/Parameters.h"
#include "g4fire/DarkBrem/G4eDarkBremsstrahlung.h"

namespace g4fire {

/**
 * @namespace darkbrem
 *
 * Extending Geant4 to dark bremsstrahlung
 *
 * In here we contain all of our framework for simulating
 * the dark bremsstrahlung process in Geant4.
 */
namespace darkbrem {

/**
 * @class APrimePhysics
 * @brief Defines basic APrime physics
 *
 * It constructs the APrime particle and links the dark brem process to the
 * electron.
 *
 * @see G4APrime
 * @see G4eDarkBremsstrahlung
 *
 * @note
 * This class basically does not do anything except
 * register the custom particle (G4APrime) and custom
 * process (G4eDarkBremsstrahlung).
 */
class APrimePhysics : public G4VPhysicsConstructor {
 public:
  /**
   * The name of this physics constructor.
   *
   * Passed into Geant4 to register this physics,
   * should not conflict with any other Geant4
   * physics.
   */
  static const std::string NAME;

  /**
   * Class constructor.
   *
   * @param params Parameters to configure the dark brem process
   */
  APrimePhysics(const fire::config::Parameters &params);

  /**
   * Class destructor.
   *
   * Nothing right now.
   */
  ~APrimePhysics() = default;

  /**
   * Construct particle.
   *
   * Insert A' into the Geant4 particle table.
   * Geant4 registers all instances derived from G4ParticleDefinition
   * and deletes them at the end of processing.
   *
   * Uses the A' mass given by the parameter APrimeMass to
   * inform the G4APrime instance what mass to use.
   *
   * @see G4APrime
   */
  void ConstructParticle();

  /**
   * Construct the process.
   *
   * Links the dark brem processs to the electron through the process manager
   * only if the dark brem process is enabled ('enable' is True).
   *
   * G4ProcessManager registers and cleans up any created processes,
   * so we can forget about it after creating it.
   *
   * @see G4eDarkBremsstrahlung
   */
  void ConstructProcess();

 private:
  /// the mass of the A' for this run
  G4double ap_mass_;

  /// is dark brem enabled for this run?
  bool enable_;

  /**
   * Dark brem params to pass to the process (if enabled)
   *
   * @note This can't be a reference because we pass it to
   * the process _after_ the configuration step from our POV
   * is done. Thus we need our own copy that won't be destroyed.
   */
  fire::config::Parameters params_;
};

}  // namespace darkbrem
}  // namespace g4fire

