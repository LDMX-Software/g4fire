#include <random>

#include <CLHEP/Units/PhysicalConstants.h>
#include <G4Event.hh>
#include <G4ThreeVector.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>

#include <g4fire/TransientInformation.h>
#include <g4fire/user/PrimaryGenerator.h>

namespace g4fire::plugins::primary_generators {

/**
 * @class MultiParticleGunPrimaryGenerator
 * @brief Generates a Geant4 event from particle gun, but can have many
 * particles
 */
class MultiParticleGunPrimaryGenerator : public user::PrimaryGenerator {
 public:
  /**
   * Constructor
   *
   * @param name the name of this generator
   * @param parameters the configuration parameters
   *
   * Parameters:
   *  vertex        : Position to shoot from (mm)
   *  momentum      : 3-vector mometum of particles (MeV)
   *  nParticles    : number of particles to shoot (mean if Poisson enabled)
   *  pdgID         : pdgID of particle to shoot
   *  enablePoisson : whether to poisson distribute the number of particles
   */
  MultiParticleGunPrimaryGenerator(const fire::config::Parameters& parameters);

  /** Destructor */
  virtual ~MultiParticleGunPrimaryGenerator();

  /**
   * Generate vertices in the Geant4 event.
   *
   * @param anEvent The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* anEvent);

  virtual void RecordConfig(fire::RunHeader& rh) const final override;

 private:
  /** Random number generator. */
  std::mt19937 rng_;
  std::poisson_distribution<std::size_t> rand_num_;

  /** The vertex position from which to fire the particles. */
  G4ThreeVector mpgVertex_;

  /** The initial momentum of the particles. */
  G4ThreeVector mpgMomentum_;

  /** Number of particles that will be fired by the gun per event. */
  double mpgNParticles_{1.};

  /** PDG ID of the particle used by the gun. */
  int mpgPdgID_{99999};

  /**
   * Flag denoting whether the number of incident particles should
   * be Poisson distributed.
   */
  bool mpgEnablePoisson_{false};

};  // MultiParticleGunPrimaryGenerator

MultiParticleGunPrimaryGenerator::MultiParticleGunPrimaryGenerator(
    const fire::config::Parameters& parameters)
    : PrimaryGenerator(parameters),
      rng_{}, // this is where a seed to the RNG would be
      rand_num_{parameters.get<int>("nParticles")} { 
  auto stlVertex{parameters.get<std::vector<double> >("vertex")};
  auto stlMomentum{parameters.get<std::vector<double> >("momentum")};
  mpgNParticles_ = parameters.get<int>("nParticles");
  mpgPdgID_ = parameters.get<int>("pdgID");
  mpgEnablePoisson_ = parameters.get<bool>("enablePoisson");

  if (stlVertex.size() != 3 or stlMomentum.size() != 3 or mpgNParticles_ <= 0) {
    throw fire::Exception("InvalidConfig",
        "Parameters passed to the Multi-Particle Gun are not valid.", false);
  }

  using CLHEP::mm;
  mpgVertex_ = G4ThreeVector(stlVertex.at(0) * mm, stlVertex.at(1) * mm,
                             stlVertex.at(2) * mm);
  using CLHEP::MeV;
  mpgMomentum_ = G4ThreeVector(stlMomentum.at(0) * MeV, stlMomentum.at(1) * MeV,
                               stlMomentum.at(2) * MeV);
}

MultiParticleGunPrimaryGenerator::~MultiParticleGunPrimaryGenerator() {}

void MultiParticleGunPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {
  int cur_mpg_pdgid = mpgPdgID_;
  G4ThreeVector cur_mpg_vertex = mpgVertex_;
  G4ThreeVector cur_mpg_momentum = mpgMomentum_;

  // current number of vertices in the event!
  int curNVertices = anEvent->GetNumberOfPrimaryVertex();

  double nInteractionsInput = mpgNParticles_;
  int nInteractions = nInteractionsInput;
  if (mpgEnablePoisson_) {
    nInteractions = 0;
    while (nInteractions == 0) {  // keep generating a random poisson until > 0,
                                  // no point in generator 0 vertices...
      nInteractions = rand_num_(rng_);
    }
  }

  // make a for loop
  for (int i = 0; i < (nInteractions - curNVertices); ++i) {
    G4PrimaryVertex* curvertex =
        new G4PrimaryVertex(cur_mpg_vertex, 0.);  // second input is t0
    // curvertex->SetPosition(0. * mm,0. * mm,-10. * mm);
    curvertex->SetWeight(1.);

    G4PrimaryParticle* primary =
        new G4PrimaryParticle(cur_mpg_pdgid, cur_mpg_momentum.x(),
                              cur_mpg_momentum.y(), cur_mpg_momentum.z());

    auto* primary_info = new PrimaryParticleInformation;
    primary_info->add("hep_evt_status",1);
    primary->SetUserInformation(primary_info);

    curvertex->SetPrimary(primary);
    anEvent->AddPrimaryVertex(curvertex);
  }
}

}  // namespace g4fire

DECLARE_GENERATOR(g4fire::plugins::primary_generators::MultiParticleGunPrimaryGenerator)
