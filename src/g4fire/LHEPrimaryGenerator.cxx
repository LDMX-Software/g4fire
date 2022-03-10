#include "g4fire/LHEPrimaryGenerator.h"

#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "fire/exception/Exception.h"

#include "g4fire/UserPrimaryParticleInformation.h"

namespace g4fire {

LHEPrimaryGenerator::LHEPrimaryGenerator(
    const std::string& name, fire::config::Parameters& params)
    : PrimaryGenerator(name, params) {
  std::string filePath = params_.get<std::string>("filePath");
  reader_ = new LHEReader(filePath);
}

LHEPrimaryGenerator::~LHEPrimaryGenerator() { delete reader_; }

void LHEPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {
  LHEEvent* lheEvent = reader_->readNextEvent();

  if (lheEvent != NULL) {
    G4PrimaryVertex* vertex = new G4PrimaryVertex();
    vertex->SetPosition(lheEvent->getVertex()[0], lheEvent->getVertex()[1],
                        lheEvent->getVertex()[2]);
    vertex->SetT0(lheEvent->getVertexTime());
    vertex->SetWeight(lheEvent->getXWGTUP());

    std::map<LHEParticle*, G4PrimaryParticle*> particleMap;

    int particleIndex = 0;
    const std::vector<LHEParticle*>& particles = lheEvent->getParticles();
    for (std::vector<LHEParticle*>::const_iterator it = particles.begin();
         it != particles.end(); it++) {
      LHEParticle* particle = (*it);

      if (particle->getISTUP() > 0) {
        G4PrimaryParticle* primary = new G4PrimaryParticle();
        if (particle->getIDUP() == -623) { /* Tungsten ion */
          G4ParticleDefinition* tungstenIonDef =
              G4IonTable::GetIonTable()->GetIon(74, 184, 0.);
          if (tungstenIonDef != NULL) {
            primary->SetParticleDefinition(tungstenIonDef);
          } else {
            throw fire::Exception("EventGenerator",
                            "Failed to find particle definition for W ion.", false);
          }
        } else {
          primary->SetPDGcode(particle->getIDUP());
        }

        primary->Set4Momentum(
            particle->getPUP(0) * GeV, particle->getPUP(1) * GeV,
            particle->getPUP(2) * GeV, particle->getPUP(3) * GeV);
        primary->SetProperTime(particle->getVTIMUP() * nanosecond);

        UserPrimaryParticleInformation* primaryInfo =
            new UserPrimaryParticleInformation();
        primaryInfo->setHepEvtStatus(particle->getISTUP());
        primary->SetUserInformation(primaryInfo);

        particleMap[particle] = primary;

        /*
         * Assign primary as daughter but only if the mother is not a DOC
         * particle.
         */
        if (particle->getMother(0) != NULL &&
            particle->getMother(0)->getISTUP() > 0) {
          G4PrimaryParticle* primaryMom = particleMap[particle->getMother(0)];
          if (primaryMom != NULL) {
            primaryMom->SetDaughter(primary);
          }
        } else {
          vertex->SetPrimary(primary);
        }
      }

      ++particleIndex;
    }

    anEvent->AddPrimaryVertex(vertex);

  } else {
    std::cout << "[ LHEPrimaryGenerator ] : Ran out of input events so run "
                 "will be aborted!"
              << std::endl;
    G4RunManager::GetRunManager()->AbortRun(true);
    anEvent->SetEventAborted();
  }

  delete lheEvent;
}

}  // namespace g4fire

DECLARE_GENERATOR(g4fire, LHEPrimaryGenerator)
