#ifndef G4FIRE_PRIMARYGENERATOR_H
#define G4FIRE_PRIMARYGENERATOR_H

#include <string>

#include "G4VPrimaryGenerator.hh"

#include "fire/config/Parameters.h"
#include "fire/factory/Factory.h"

class G4Event;

namespace g4fire::user {

/**
 * @brief Interface that defines a simulation primary generator.
 *
 * This class inherits from the Geant4 Primary Genertor template,
 * and is used as a common reference for all of the other PrimaryGenerators.
 */
class PrimaryGenerator : public G4VPrimaryGenerator {
 public:
  /**
   * Factory for primary generators
   */
  using Factory = ::fire::factory::Factory<PrimaryGenerator,std::unique_ptr<PrimaryGenerator>,const fire::config::Parameters&>;
 public:
  /**
   * Configure the primary generator
   *
   * @param name Name given the to class instance.
   * @param The parameter set used to configure this class.
   */
  PrimaryGenerator(const fire::config::Parameters &params)
    : G4VPrimaryGenerator() {}

  /// Destructor
  virtual ~PrimaryGenerator() = default;

  /**
   * Generate a primary vertex.
   *
   * This is a pure virtual method and must be defined by a deriving class.
   *
   * @param event The Geant4 event to which the primary vertex will be added.
   */
  virtual void GeneratePrimaryVertex(G4Event *event) = 0;
}; // PrimaryGenerator
} // namespace g4fire::user

/**
 * @macro DECLARE_GENERATOR
 *
 * Defines a builder for the declared class and then registers the class as a
 * generator with the PrimaryGeneratorManager.
 */
#define DECLARE_GENERATOR(NS, CLASS)                                           \
  namespace { \
  auto v = ::g4fire::user::PrimaryGenerator::Factory::get().declare<CLASS>(); \
  }

#endif // G4FIRE_PRIMARYGENERATOR_H
