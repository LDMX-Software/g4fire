#include "g4fire/DetectorConstruction.h"

#include "g4fire/PluginFactory.h"
#include "g4fire/XsecBiasingOperator.h"

namespace g4fire {

namespace logical_volume_tests {

/**
 * isInEcal
 *
 * Check that the passed volume is inside the ECal
 *
 * @TODO this is _horrible_
 * can we get an 'ecal' and 'hcal' region instead
 * of just a 'CalorimeterRegion' region?
 *
 * @param vol G4LogicalVolume to check
 * @param vol_to_bias UNUSED name of volume to bias
 */
static bool isInEcal(G4LogicalVolume* vol, const std::string& vol_to_bias) {
  G4String vol_name = vol->GetName();
  return ((vol_name.contains("Si") || vol_name.contains("W") ||
           vol_name.contains("PCB") || vol_name.contains("CFMix") ||
           vol_name.contains("Al")) &&
          vol_name.contains("volume"));
}

/**
 * isInHcal
 *
 * Check that the passed volume is inside the HCal
 *
 * @param vol G4LogicalVolume to check
 * @param vol_to_bias UNUSED name of volume to bias
 */
static bool isInHcal(G4LogicalVolume* vol, const std::string& vol_to_bias) {
  G4String vol_name = vol->GetName();
  return ((vol_name.contains("abso2") || vol_name.contains("abso3") ||
           vol_name.contains("ScintBox") || vol_name.contains("absoBox")) &&
          vol_name.contains("volume"));
}

/**
 * isInEcalOld
 *
 * This is the old method for checking if the passed volume was inside the ECal
 * and only looks for tungsten or silicon layers.
 *
 * @note Deprecating soon (hopefully).
 *
 * @param vol G4LogicalVolume to check
 * @param vol_to_bias UNUSED name of volume to bias
 */
static bool isInEcalOld(G4LogicalVolume* vol, const std::string& vol_to_bias) {
  G4String vol_name = vol->GetName();
  return ((vol_name.contains("Si") || vol_name.contains("W")) &&
          vol_name.contains("volume"));
}

/**
 * isInTargetRegion
 *
 * Check if the passed volume is inside the target region.
 *
 * @param vol G4LogicalVolume to check
 * @param vol_to_bias UNUSED name of volume to bias
 */
static bool isInTargetRegion(G4LogicalVolume* vol,
                             const std::string& vol_to_bias) {
  auto region = vol->GetRegion();
  return (region and region->GetName().contains("target"));
}

/**
 * isInTargetRegion
 *
 * Check if the passed volume is inside the target volume.
 *
 * @note This leaves out the trig scint modules inside the target region.
 *
 * @param vol G4LogicalVolume to check
 * @param vol_to_bias UNUSED name of volume to bias
 */
static bool isInTargetOnly(G4LogicalVolume* vol,
                           const std::string& vol_to_bias) {
  return vol->GetName().contains("target");
}

/**
 * nameContains
 *
 * Check if the passed volume has a name containing the
 * name of the volume to bias.
 *
 * @note This is the default if we don't recognize
 * the volume to bias that is requested.
 *
 * @param vol G4LogicalVolume to check
 * @param vol_to_bias name of volume to bias
 */
static bool nameContains(G4LogicalVolume* vol, const std::string& vol_to_bias) {
  return vol->GetName().contains(vol_to_bias);
}

/**
 * Define the type of all these functional tests.
 *
 * Used below when determining which test to use.
 */
typedef bool (*Test)(G4LogicalVolume*, const std::string&);

}  // namespace logical_volume_tests

DetectorConstruction::DetectorConstruction(
    g4fire::geo::Parser* parser, fire::config::Parameters& params,
    ConditionsInterface& ci)
    : parser_(parser) {
  params_ = params;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  return parser_->GetWorldVolume();
}

void DetectorConstruction::ConstructSDandField() {
  // Biasing operators were created in RunManager::setupPhysics
  //  which is called before G4RunManager::Initialize
  //  which is where this method ends up being called.

  auto bops{g4fire::PluginFactory::getInstance().getBiasingOperators()};
  for (g4fire::XsecBiasingOperator* bop : bops) {
    logical_volume_tests::Test include_vol_test{nullptr};
    if (bop->getVolumeToBias().compare("ecal") == 0) {
      include_vol_test = &logical_volume_tests::isInEcal;
    } else if (bop->getVolumeToBias().compare("old_ecal") == 0) {
      include_vol_test = &logical_volume_tests::isInEcalOld;
    } else if (bop->getVolumeToBias().compare("target") == 0) {
      include_vol_test = &logical_volume_tests::isInTargetOnly;
    } else if (bop->getVolumeToBias().compare("target_region") == 0) {
      include_vol_test = &logical_volume_tests::isInTargetRegion;
    } else if (bop->getVolumeToBias().compare("hcal") == 0) {
      include_vol_test = &logical_volume_tests::isInHcal;
    } else {
      std::cerr << "[ DetectorConstruction ] : "
                << "WARN - Requested volume to bias '" << bop->getVolumeToBias()
                << "' is not recognized. Will attach volumes based on if their"
                << " name contains the volume to bias." << std::endl;
      include_vol_test = &logical_volume_tests::nameContains;
    }

    for (G4LogicalVolume* volume : *G4LogicalVolumeStore::GetInstance()) {
      auto volume_name = volume->GetName();
      if (include_vol_test(volume, bop->getVolumeToBias())) {
        bop->AttachTo(volume);
        std::cout << "[ DetectorConstruction ]: "
                  << "Attaching biasing operator " << bop->GetName()
                  << " to volume " << volume->GetName() << std::endl;
      }  // BOP attached to target or ecal
    }    // loop over volumes
  }      // loop over biasing operators
}
}  // namespace g4fire
