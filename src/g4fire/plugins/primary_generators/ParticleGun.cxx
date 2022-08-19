#include <g4fire/user/PrimaryGenerator.h>

#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4ParticleGun.hh>

namespace g4fire::plugins::primary_generators {

/**
 * @brief Class that extends the functionality of G4ParticleGun.
 */
class ParticleGun : public user::PrimaryGenerator {
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
  ParticleGun(const fire::config::Parameters& params);

  /// Destructor
  ~ParticleGun() = default;

  /**
   * Generate the primary vertices in the Geant4 event.
   *
   * @param event The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

  virtual void RecordConfig(fire::RunHeader& rh) const final override;
 private:
  /**
   * The actual Geant4 implementation of the ParticleGun
   *
   * mutable because G4 has crap const correctness
   */
  mutable G4ParticleGun gun_;

  /**
   * LDMX Verbosity for this generator
   */
  int verbosity_;

};  // ParticleGun

ParticleGun::ParticleGun(const fire::config::Parameters &params)
    : PrimaryGenerator(params) {
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

void ParticleGun::RecordConfig(fire::RunHeader& rh) const {
  rh.set<float>("ParticleGun Position X", gun_.GetParticlePosition().x());
}

} // namespace g4fire::plugins::primary_generators

DECLARE_GENERATOR(g4fire::plugins::primary_generators::ParticleGun);
