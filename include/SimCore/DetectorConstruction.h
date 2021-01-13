
#ifndef SIMCORE_DETECTORCONSTRUCTION_H_
#define SIMCORE_DETECTORCONSTRUCTION_H_

// Geant4
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VUserDetectorConstruction.hh"

// LDMX
#include "Framework/Configure/Parameters.h"
#include "SimCore/ConditionsInterface.h"
#include "SimCore/AuxInfoReader.h"

namespace simcore {

/**
 * @class DetectorConstruction
 * @brief Implements the Geant4 detector construction
 *
 * @note
 * This class reads in a detector description from a GDML file
 * using the basic <i>G4GDMLParser</i> and instantiates supplemental
 * information using the AuxInfoReader.
 *
 * @see AuxInfoReader
 */
class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  /**
   * Class constructor.
   * @param theParser GDML parser defining the geometry.
   */
  DetectorConstruction(G4GDMLParser *theParser, 
                       const framework::config::Parameters &parameters,
                       ConditionsInterface &ci);

  /**
   * Class destructor.
   */
  virtual ~DetectorConstruction();

  /**
   * Construct the detector.
   *
   * This is called in G4RunManager::InitializeGeometry()
   * and is comes *before* ConstructSDandField().
   *
   * @return The top volume of the detector.
   */
  G4VPhysicalVolume *Construct();

  /**
   * Construct sensitive detectors and magnetic field.
   *
   * This is called in G4RunManager::InitializeGeometry()
   * and is comes *after* Construct(), so we can assume
   * logical volumes are created.
   *
   * We use this opportunity to not only construct our sensitive
   * detectors, but also to attach the biasing operators to
   * the logical volumes that they should be attached to.
   */
  void ConstructSDandField();

  /**
   * Get the detector header.
   * @return The detector header.
   */
  ldmx::DetectorHeader *getDetectorHeader() {
    return auxInfoReader_->getDetectorHeader();
  }

 private:
  /**
   * The GDML parser defining the detector.
   */
  G4GDMLParser *parser_;

  /**
   * The auxiliary GDML info reader.
   */
  AuxInfoReader *auxInfoReader_;

  /// The set of parameters used to configure this class
  const framework::config::Parameters& parameters_;

  /// Handle to the interface for conditions
  ConditionsInterface& conditions_interface_;
};

}  // namespace simcore

#endif
