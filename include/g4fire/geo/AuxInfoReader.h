#pragma once

#include "G4GDMLParser.hh"

//#include "DetDescr/DetectorHeader.h"

#include "fire/config/Parameters.h"

#include "g4fire/ConditionsInterface.h"

namespace g4fire::geo {

/**
 * @brief Reads auxiliary information from GDML userinfo block
 *
 * @note
 * This class reads information to define a detector header block, sensitive
 * detectors, visualization attributes, magnetic fields, detector IDs, and
 * detector regions from the userinfo block of a GDML file.  These objects are
 * then assigned to the appropriate logical volumes which have <i>auxiliary</i>
 * tags that reference these objects by name.
 */
class AuxInfoReader {
public:
  /**
   * Class constructor.
   * @param parser The GDML parser.
   * @param ps configuration parameters
   */
  AuxInfoReader(G4GDMLParser *parser, fire::config::Parameters ps,
                ConditionsInterface &ci);

  /**
   * Class destructor.
   */
  ~AuxInfoReader() = default;

  /**
   * Read the global auxiliary information from the auxinfo block.
   */
  void readGlobalAuxInfo();

  /**
   * Assign auxiliary info to volumes such as sensitive detectors.
   */
  void assignAuxInfoToVolumes();

  /**
   * Get the detector header that was created from the userinfo block.
   * @return The detector header.
   */
  // ldmx::DetectorHeader *getDetectorHeader() { return detector_header_; }

private:
  /**
   * Create a sensitive detector from GDML data.
   * @param sd_type The type of the sensitive detector.
   * @param aux_info_list The aux info defining the sensitive detector.
   */
  void createSensitiveDetector(G4String sd_type,
                               const G4GDMLAuxListType *aux_info_list);

  /**
   * Create a magnetic field from GDML data.
   * @param name The name of the magnetic field.
   * @param aux_info_list The aux info defining the magnetic field.
   */
  void createMagneticField(G4String name,
                           const G4GDMLAuxListType *aux_info_list);

  /**
   * Create a detector region from GDML data.
   * @param name The name of the detector region.
   * @param aux_info_list The aux info defining the detector region.
   */
  void createRegion(G4String name, const G4GDMLAuxListType *aux_info_list);

  /**
   * Create visualization attributes from GDML data.
   * @param name The name of the visualization attributes.
   * @param aux_info_list The aux info defining the visualization attributes.
   */
  void createVisAttributes(G4String name,
                           const G4GDMLAuxListType *aux_info_list);

  /**
   * Create the detector header from the global auxinfo.
   * @param detector_version The aux value with the detector version.
   * @param aux_info_list The aux info with the detector header information.
   */
  void createDetectorHeader(G4String detector_version,
                            const G4GDMLAuxListType *aux_info_list);

private:
  /// The GDML Parser
  G4GDMLParser *parser_;

  /// The GDML expression evaluator.
  std::unique_ptr<G4GDMLEvaluator> eval_;

  /**
   * Detector header with name and version.
   */
  // ldmx::DetectorHeader *detector_header_{nullptr};

  /// Configuration params
  fire::config::Parameters params_;

  /// ConditionsInterface
  ConditionsInterface &conditions_intf_;
};

} // namespace g4fire::geo
