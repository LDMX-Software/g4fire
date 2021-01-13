#include "SimCore/DetectorConstruction.h"

#include "Framework/Exception/Exception.h" 
#include "SimCore/XsecBiasingOperator.h"
#include "SimCore/PluginFactory.h"

namespace simcore {

DetectorConstruction::DetectorConstruction(G4GDMLParser* theParser,
                                           const framework::config::Parameters& parameters,
                                           ConditionsInterface& ci)
    : parser_(theParser),
      conditions_interface_{ci},
      auxInfoReader_(new AuxInfoReader(theParser, parameters, ci)),
      parameters_(parameters) {}

DetectorConstruction::~DetectorConstruction() { delete auxInfoReader_; }

G4VPhysicalVolume* DetectorConstruction::Construct() {
  auxInfoReader_->readGlobalAuxInfo();
  auxInfoReader_->assignAuxInfoToVolumes();
  return parser_->GetWorldVolume();
}

void DetectorConstruction::ConstructSDandField() {

  // handle to the factory
  auto& factory{simcore::PluginFactory::getInstance()};

  // first, let's create our sensitive detectors
  auto sens_dets{parameters_.getParameter<std::vector<framework::config::Parameters>>("sensitive_detectors",{})};
  for (auto& det : sens_dets) {
    factory.createSensitiveDetector(
        det.getParameter<std::string>("class_name"),
        det.getParameter<std::string>("instance_name"),
        conditions_interface_, det);
  }

  // one loop over the logical volumes to attach biasing operators
  // and sensitive detectors
  for (G4LogicalVolume* volume : *G4LogicalVolumeStore::GetInstance()) {
    // Go through sensitive detectors and see if any of them
    //  should be attached to this volume
    for (SensitiveDetector* det : factory.getSensitiveDetectors()) {
      if (det->isSensDet(volume)) volume->SetSensitiveDetector(det);
    }  // loop over sensitive detectors

    // Biasing operators were created in RunManager::setupPhysics
    //  which is called before RunManager::Initialize
    //  which is where this method ends up being called.
    for (XsecBiasingOperator* bop : factory.getBiasingOperators()) {
      if (bop->getVolumeToBias().compare("ecal") == 0) {
        G4String volumeName = volume->GetName();
        if ((volumeName.contains("Wthick") || volumeName.contains("Si") ||
             volumeName.contains("W") || volumeName.contains("PCB") ||
             volumeName.contains("CFMix") || volumeName.contains("Al")) &&
            volumeName.contains("volume")) {
          bop->AttachTo(volume);
          std::cout << "[ DetectorConstruction ]: "
                    << "Attaching biasing operator " << bop->GetName()
                    << " to volume " << volume->GetName() << std::endl;
        }  // volume matches pattern for ecal volumes
      } else if (bop->getVolumeToBias().compare("target") == 0) {
        auto region = volume->GetRegion();
        if (region and region->GetName().contains("target")) {
          bop->AttachTo(volume);
          std::cout << "[ DetectorConstruction ]: "
                    << "Attaching biasing operator " << bop->GetName()
                    << " to volume " << volume->GetName() << std::endl;
        }  // volume is in target region
      }    // BOP attached to target or ecal
    }      // loop over biasing operators

  }        // loop over volumes
}

}  // namespace simcore
