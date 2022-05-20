#ifndef G4FIRE_GAMMAPHYSICS_H
#define G4FIRE_GAMMAPHYSICS_H 

#include "G4GammaConversionToMuons.hh"
#include "G4ProcessManager.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4VProcess.hh"

namespace g4fire {

/**
 *
 * @note
 * Currently adds gamma -> mumu reaction using the
 * <i>G4GammaConversionToMuons</i> process. Also changes ordering of
 * gamma processes such that photonNuclear and GammaToMuMu are called first.
 */
class GammaPhysics : public G4VPhysicsConstructor {
 public:
  /**
   * Class constructor.
   * @param name The name of the physics.
   */
  GammaPhysics(const G4String& name = "GammaPhysics");

  /**
   * Class destructor.
   */
  ~GammaPhysics() = default;

  /**
   * Construct particles (no-op).
   */
  void ConstructParticle() {}

  /**
   * Construct the process (gamma to muons).
   */
  void ConstructProcess();

 private:
  /**
   * The gamma to muons process.
   */
  G4GammaConversionToMuons gamma_conv_process;
};

}  // namespace g4fire

#endif
