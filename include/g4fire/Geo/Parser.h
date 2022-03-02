#ifndef SIMCORE_GEO_GEOPARSER_H_
#define SIMCORE_GEO_GEOPARSER_H_

//---< C++ >---//
#include <functional>

//---< Framework >---//
#include "Framework/Configure/Parameters.h"

//---< g4fire >---//
#include "g4fire/ConditionsInterface.h"

//---< Geant4 >---//
#include "G4VPhysicalVolume.hh"

namespace g4fire {
namespace geo {

/**
 * Interface for a parser used to load a geometry into Geant4.
 *
 * This will be used by a parser factory class to spawn an instance of the
 * desired parser.
 */
class Parser {
 public:
  /**
   * Pure virtual method used to retrieve the G4VPhysicalVolume associated with
   * the most top-level (world) volume.
   *
   * @return The world volume.
   */
  virtual G4VPhysicalVolume *GetWorldVolume() = 0;

  /**
   * Get the name of the parsed detector.
   *
   * This name is typically extracted from the file containing the detector
   * description.
   *
   * @return The name of the detector.
   */
  virtual std::string getDetectorName() = 0;

  /**
   * Parse the detector geometry and read it into memory.
   */
  virtual void read() = 0;

};  // Parser

using createFunc =
    std::function<Parser *(framework::config::Parameters &parameters,
                           g4fire::ConditionsInterface &ci)>;

}  // namespace geo
}  // namespace g4fire

#endif  // SIMCORE_GEO_GEOPARSER_H_
