#include "g4fire/GammaPhysics.h"

namespace g4fire {

GammaPhysics::GammaPhysics(const G4String& name)
    : G4VPhysicsConstructor(name) {}

// needed for GEANT4 10.3.0 and later
#ifndef aParticleIterator
#define aParticleIterator \
  ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
#endif

void GammaPhysics::ConstructProcess() {
  aParticleIterator->reset();

  // Loop through all of the particles and find the "gamma".
  while ((*aParticleIterator)()) {
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particle_name = particle->GetParticleName();

    if (particle_name == "gamma") {
      // Get the process list associated with the gamma.
      G4ProcessVector* v_process = pmanager->GetProcessList();

      // Search the list for the process "photoNuclear".  When found,
      // change the calling order so photonNuclear is called before
      // EM processes. The biasing operator needs the photonNuclear
      // process to be called first because the cross-section is
      // needed to bias down other process.
      for (int ip = 0; ip < v_process->size(); ++ip) {
        G4String process_name = (*v_process)[ip]->GetProcessName();
        if (process_name == "photonNuclear") {
          pmanager->SetProcessOrderingToFirst((*v_process)[ip],
                                              G4ProcessVectorDoItIndex::idxAll);
        }
      }

      // Add the gamma -> mumu to the physics list.
      pmanager->AddDiscreteProcess(&gamma_conv_process);
    }
  }
}
}  // namespace g4fire
