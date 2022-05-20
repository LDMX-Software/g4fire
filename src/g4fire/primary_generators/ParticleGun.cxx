#include "g4fire/ParticleGun.h"

#include <memory>

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

namespace g4fire {

ParticleGun::ParticleGun(const std::string &name,
                         fire::config::Parameters &params)
    : PrimaryGenerator(name, params) {
  verbosity_ = params.get<int>("verbosity");

  auto particle_table{G4ParticleTable::GetParticleTable()};

  auto particle{params.get<std::string>("particle")};
  if (auto particle_def{particle_table->FindParticle(particle)};
      particle_def != 0) {
    if (verbosity_ > 1) {
      std::cout << "[ ParticleGun ] : Firing particle of type " << particle
                << std::endl;
    }
    gun_.SetParticleDefinition(particle_def);
  }

  auto energy{params.get<double>("energy")};
  if (verbosity_ > 1) {
    std::cout << "[ ParticleGun ] : Setting energy to " << energy * GeV
              << std::endl;
  }
  gun_.SetParticleEnergy(energy * GeV);

  auto position{params.get<std::vector<double>>("position")};
  if (!position.empty()) {
    G4ThreeVector pVec(position[0] * mm, position[1] * mm, position[2] * mm);
    if (verbosity_ > 1) {
      std::cout << "[ ParticleGun ] : position " << pVec << std::endl;
    }
    gun_.SetParticlePosition(pVec);
  }

  auto time{params.get<double>("time")};
  if (time < 0)
    time = 0.0;
  if (verbosity_ > 1) {
    std::cout << "[ ParticleGun ] : Setting particle time  to " << time
              << std::endl;
  }
  gun_.SetParticleTime(time * ns);

  auto direction{params.get<std::vector<double>>("direction")};
  if (!direction.empty()) {
    G4ThreeVector dVec(direction[0], direction[1], direction[2]);
    if (verbosity_ > 1) {
      std::cout << "[ ParticleGun ] : direction " << dVec.unit() << std::endl;
    }
    gun_.SetParticleMomentumDirection(dVec);
  }
}

void ParticleGun::GeneratePrimaryVertex(G4Event *event) {
  // Call G4 class method to generate primaries.
  gun_.GeneratePrimaryVertex(event);
}

} // namespace g4fire

DECLARE_GENERATOR(g4fire, ParticleGun)
