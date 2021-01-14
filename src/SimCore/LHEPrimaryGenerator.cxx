/**
 * @file LHEPrimaryGenerator.cxx
 * @brief Implementation file for LHEPrimaryGenerator
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "SimCore/LHEPrimaryGenerator.h"

// Geant4
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"

// LDMX
#include "SimCore/UserPrimaryParticleInformation.h"
#include "Framework/Configure/Parameters.h"
#include "Framework/Exception/Exception.h"

namespace ldmx {

    LHEPrimaryGenerator::LHEPrimaryGenerator(const std::string& name , Parameters& parameters)
        : PrimaryGenerator( name , parameters ) {

        std::string filePath = parameters_.getParameter< std::string >( "filePath" );
        reader_ = new LHEReader( filePath );

    }

    LHEPrimaryGenerator::~LHEPrimaryGenerator() {
        delete reader_;
    }

    void LHEPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {

        LHEEvent* lheEvent = reader_->readNextEvent();

        if (lheEvent != NULL) {

            G4PrimaryVertex* vertex = new G4PrimaryVertex();
            vertex->SetPosition(lheEvent->getVertex()[0],lheEvent->getVertex()[1],lheEvent->getVertex()[2]);
            vertex->SetWeight(lheEvent->getXWGTUP());

            std::map<LHEParticle*, G4PrimaryParticle*> particleMap;

            int particleIndex = 0;
            const std::vector<LHEParticle*>& particles = lheEvent->getParticles();
	    //std::cout << "[[whitbeck]] particles: " << particles.size() << std::endl;
            for (std::vector<LHEParticle*>::const_iterator it = particles.begin(); it != particles.end(); it++) {

                LHEParticle* particle = (*it);

		/*
		std::cout << "next particle: " << std::endl;
		std::cout << "status: " << particle->getISTUP() << std::endl;
		std::cout << "id: " << particle->getIDUP() << std::endl;
		std::cout << "px,py,pz,E: " << particle->getPUP(0)/10. << " " << particle->getPUP(1)/10. << " " << particle->getPUP(2)/10. << " " << particle->getPUP(3)/10. << std::endl;
		std::cout << "parent1: " << particle->getMother(0) << std::endl;
		std::cout << "parent2: " << particle->getMother(1) << std::endl;
		*/
		
                if (particle->getISTUP() > 0 && particle->getIDUP() != 622) {


		  //std::cout << "status: good" << std::endl;
		  
		  G4PrimaryParticle* primary = new G4PrimaryParticle();
                    if (abs(particle->getIDUP()) == -623) { /* Tungsten ion */
		        //std::cout << "Tungsten ion found" << std::endl;
                        G4ParticleDefinition* tungstenIonDef = G4IonTable::GetIonTable()->GetIon(74, 184, 0.);
                        if (tungstenIonDef != NULL) {
                            primary->SetParticleDefinition(tungstenIonDef);
                        } else {
                            EXCEPTION_RAISE( "EventGenerator" ,
                                    "Failed to find particle definition for W ion." );
                        }
                    } else {
                        primary->SetPDGcode(particle->getIDUP());
                    }

                    primary->Set4Momentum(particle->getPUP(0)/10. * GeV, 
                                          particle->getPUP(1)/10. * GeV, 
                                          particle->getPUP(2)/10. * GeV, 
                                          particle->getPUP(3)/10. * GeV);
                    primary->SetProperTime(particle->getVTIMUP() * nanosecond);

                    UserPrimaryParticleInformation* primaryInfo = new UserPrimaryParticleInformation();
                    primaryInfo->setHepEvtStatus(particle->getISTUP());
                    primary->SetUserInformation(primaryInfo);

                    particleMap[particle] = primary;

                    /*
                     * Assign primary as daughter but only if the mother is not a DOC particle.
                     */
                    if (particle->getMother(0) != NULL && particle->getMother(0)->getISTUP() > 0 && particle->getMother(0)->getIDUP() != 622) {
		        //std::cout << "found final state mother" << std::endl;
                        G4PrimaryParticle* primaryMom = particleMap[particle->getMother(0)];
                        if (primaryMom != NULL) {
                            primaryMom->SetDaughter(primary);
                        }
                    } else {
		        //std::cout << "found final state PV" << std::endl;
                        vertex->SetPrimary(primary);
                    }

                } 

                ++particleIndex;
            }

            anEvent->AddPrimaryVertex(vertex);

        } else {
            std::cout << "[ LHEPrimaryGenerator ] : Ran out of input events so run will be aborted!" << std::endl;
            G4RunManager::GetRunManager()->AbortRun(true);
            anEvent->SetEventAborted();
        }

        delete lheEvent;
    }

}

DECLARE_GENERATOR( ldmx , LHEPrimaryGenerator )
