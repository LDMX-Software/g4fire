#include "g4fire/user/PrimaryGenerator.h"

#include <G4Event.hh>
#include <G4IonTable.hh>
#include <G4PhysicalConstants.hh>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>

#include <fire/exception/Exception.h>

#include "g4fire/lhe/LHEEvent.h"
#include "g4fire/TransientInformation.h"

namespace g4fire::plugins::primary_generators {

class LHEPrimaryGenerator : public user::PrimaryGenerator {
  std::ifstream lhe_file_;
 public:
  LHEPrimaryGenerator(const fire::config::Parameters& p);
  ~LHEPrimaryGenerator() = default;
  void GeneratePrimaryVertex(G4Event* event) final override;
  void RecordConfig(fire::RunHeader& rh) const final override;
};

LHEPrimaryGenerator::LHEPrimaryGenerator(
    const fire::config::Parameters& params)
    : PrimaryGenerator(params) {
  auto fp{params.get<std::string>("file_path")};
  lhe_file_.open(fp);
  if (not lhe_file_.is_open()) {
    throw fire::Exception("BadFile",
        "Unable to open '"+fp+"'.");
  }
}

void LHEPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {
  LHEEvent lhe_event;
  if (not (lhe_file_ >> lhe_event)) {
    std::cout << "[ LHEPrimaryGenerator ] : Ran out of input events so run "
                 "will be aborted!"
              << std::endl;
    G4RunManager::GetRunManager()->AbortRun(true);
    anEvent->SetEventAborted();
    return;
  }
  
  G4PrimaryVertex* vertex = new G4PrimaryVertex();
  vertex->SetPosition(lhe_event.getVertex()[0], lhe_event.getVertex()[1],
                      lhe_event.getVertex()[2]);
  vertex->SetT0(lhe_event.getVertexTime());
  vertex->SetWeight(lhe_event.getXWGTUP());


  std::map<int, G4PrimaryParticle*> created_primaries;
  const std::vector<LHEParticle>& lhe_particles = lhe_event.getParticles();
  for (std::size_t i_particle{0}; i_particle < lhe_particles.size(); ++i_particle) {
    const auto& particle{lhe_particles.at(i_particle)};
    if (particle.getISTUP() > 0) {
      G4PrimaryParticle* primary = new G4PrimaryParticle();
      if (particle.getIDUP() == -623) { /* Tungsten ion */
        G4ParticleDefinition* tungstenIonDef =
            G4IonTable::GetIonTable()->GetIon(74, 184, 0.);
        if (tungstenIonDef != NULL) {
          primary->SetParticleDefinition(tungstenIonDef);
        } else {
          throw fire::Exception("EventGenerator",
                          "Failed to find particle definition for W ion.", false);
        }
      } else {
        primary->SetPDGcode(particle.getIDUP());
      }

      primary->Set4Momentum(
          particle.getPUP(0) * GeV, particle.getPUP(1) * GeV,
          particle.getPUP(2) * GeV, particle.getPUP(3) * GeV);
      primary->SetProperTime(particle.getVTIMUP() * nanosecond);

      auto* primaryInfo = new PrimaryParticleInformation;
      primaryInfo->add("HepEvtStatus",particle.getISTUP());
      primary->SetUserInformation(primaryInfo);

      created_primaries[particle.getISTUP()] = primary;

      /**
       * Assign primary as daughter but only if the mother is not a DOC
       * particle.
       */
      if (particle.getMother(0) != NULL &&
          particle.getMother(0)->getISTUP() > 0) {
        G4PrimaryParticle* primaryMom = created_primaries[particle.getMother(0)->getISTUP()];
        if (primaryMom != NULL) {
          primaryMom->SetDaughter(primary);
        }
      } else {
        vertex->SetPrimary(primary);
      }
    }
  }

  anEvent->AddPrimaryVertex(vertex);
}

}  // namespace g4fire

DECLARE_GENERATOR(g4fire::plugins::primary_generators::LHEPrimaryGenerator)
