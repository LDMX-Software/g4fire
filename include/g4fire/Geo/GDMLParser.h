#ifndef SIMCORE_GEO_GDMLPARSER_H
#define SIMCORE_GEO_GDMLPARSER_H

//---< Geant4 >---//
#include "G4GDMLParser.hh"

//---< Framework >---//
#include "Framework/Configure/Parameters.h"

//---< g4fire >---//
#include "g4fire/Geo/AuxInfoReader.h"
#include "g4fire/Geo/Parser.h"

// Forward Declarations
class G4VPhysicalVolume;

namespace g4fire {
namespace geo {

/**
 * Parse GDML files, build the geometry in memory and load it into Geant4.
 *
 * This class extends the interface Parser which allows creation of the
 * parser at runtime via a factory.
 */
class GDMLParser : public Parser {
 public:
  /**
   * Default constructor.
   *
   * @param parameters The parameters used to configure this parser.
   * @param ci Interface that allows access to the conditions.
   */
  GDMLParser(framework::config::Parameters &parameters,
             g4fire::ConditionsInterface &ci);

  /// Default destructor
  ~GDMLParser() = default;

  /**
   * Retrieve the G4VPhysicalVolume associated with the most top-level
   * (world) volume.
   *
   * @return The world volume.
   */
  G4VPhysicalVolume *GetWorldVolume() final override;

  /**
   * Get the name of the parsed detector.
   *
   * This name is typically extracted from the file containing the detector
   * description.
   *
   * @return The name of the detector.
   */
  std::string getDetectorName() final override { return detector_name_; }

  /**
   * Parse the detector geometry and read it into memory.
   */
  void read() final override;

  /**
   * Create an instance of this parser.
   */
  static Parser *create(framework::config::Parameters &parameters,
                        g4fire::ConditionsInterface &ci) {
    return new GDMLParser(parameters, ci);
  }

 private:
  /// The GDML parser.
  std::unique_ptr<G4GDMLParser> parser_;

  /// The auxiliary info reader
  std::unique_ptr<g4fire::geo::AuxInfoReader> info_;

  /// The parameters used to configure this parser
  framework::config::Parameters parameters_;

  /// The name of the parsed detector
  std::string detector_name_{""};

};  // GDMLParser
}  // namespace geo
}  // namespace g4fire

#endif  // SIMCORE_GEO_GDMLPARSER_H
