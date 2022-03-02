#ifndef SIMCORE_DETECTORCONSTRUCTION_H
#define SIMCORE_DETECTORCONSTRUCTION_H

//---< Geant4 >---//
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VUserDetectorConstruction.hh"

//---< Framework >---//
#include "Framework/Configure/Parameters.h"

//---< g4fire >---//
#include "g4fire/Geo/Parser.h"

// Forward declaration
namespace g4fire::geo {
class Parser;
}

namespace g4fire {

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
   * Constructor.
   *
   * @param parser Parser used to parse the geometry into memory.
   * @param parameters The parameters used to configure this class.
   * @param ci The conditions needed to build the detector.
   */
  DetectorConstruction(g4fire::geo::Parser *parser,
                       framework::config::Parameters &parameters,
                       ConditionsInterface &ci);

  /**
   * Class destructor.
   */
  ~DetectorConstruction() = default;

  /**
   * Construct the detector.
   * @return The top volume of the detector.
   */
  G4VPhysicalVolume *Construct();

  /**
   */
  void ConstructSDandField();

  /**
   * @return The name of this detector. This is extracted from the
   *	description file used to build this detector.
   */
  std::string getDetectorName() { return parser_->getDetectorName(); }

 private:
  /// The parser used to load the detector into memory.
  g4fire::geo::Parser *parser_;

  /// The set of parameters used to configure this class
  framework::config::Parameters parameters_;
};  // DetectorConstruction
}  // namespace g4fire

#endif  // SIMCORE_DETECTORCONSTRUCTION_H
