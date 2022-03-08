#include "g4fire/DarkBrem/APrimePhysics.h"

#include "g4fire/DarkBrem/G4APrime.h"

#include "G4Electron.hh"
#include "G4ProcessManager.hh"

namespace g4fire {
namespace darkbrem {

const std::string APrimePhysics::NAME = "APrime";

APrimePhysics::APrimePhysics(const fire::config::Parameters &params)
    : G4VPhysicsConstructor(APrimePhysics::NAME), parameters_{params} {
  ap_mass_ = parameters_.get<double>("ap_mass", 0.) * MeV;
  enable_ = parameters_.get<bool>("enable", false);
}

void APrimePhysics::ConstructParticle() {
  /**
   * Insert A-prime into the Geant4 particle table.
   * For now we flag it as stable.
   *
   * Geant4 registers all instances derived from G4ParticleDefinition and
   * deletes them at the end of the run.
   */
  G4APrime::APrime(ap_mass_);
}

void APrimePhysics::ConstructProcess() {
  // add process to electron if LHE file has been provided
  if (enable_) {
    /*
     * In G4 speak, a "discrete" process is one that only happens at the end of
     * steps. we want the DB to be discrete because it is not a "slow braking"
     * like ionization, the electron suddenly has the interaction and loses a
     * lot of its energy.
     *
     * The first argument to this function is the process we are adding.
     *      The process manager handles cleaning up the processes,
     *      so we just give it a new pointer.
     * The second argument is the "ordering" index.
     *      This index determines when the process is called w.r.t. the other
     * processes that could be called at the end of the step. Not providing the
     * second argument means that the ordering index is given a default value of
     * 1000 which seems to be safely above all the internal/default processes.
     */
    G4Electron::ElectronDefinition()->GetProcessManager()->AddDiscreteProcess(
        new G4eDarkBremsstrahlung(parameters_));
  }
}

} // namespace darkbrem
} // namespace g4fire
