#ifndef G4FIRE_PRIMARYGENERATOR_H
#define G4FIRE_PRIMARYGENERATOR_H

#include <string>

#include "G4VPrimaryGenerator.hh"

#include "fire/config/Parameters.h"

class G4Event;

namespace g4fire {

class PrimaryGenerator;

typedef PrimaryGenerator *
PrimaryGeneratorBuilder(const std::string &name,
                        fire::config::Parameters &params);

/**
 * @brief Interface that defines a simulation primary generator.
 *
 * This class inherits from the Geant4 Primary Genertor template,
 * and is used as a common reference for all of the other PrimaryGenerators.
 */
class PrimaryGenerator : public G4VPrimaryGenerator {
 public:
  /**
   * Constructor.
   *
   * @param name Name given the to class instance.
   * @param The parameter set used to configure this class.
   */
  PrimaryGenerator(const std::string &name, fire::config::Parameters &params);

  /// Destructor
  virtual ~PrimaryGenerator() = default;

  /**
   * Method used to register a user action with the manager.
   *
   * @param class_name Name of the class instance
   * @param builder The builder used to create an instance of this class.
   */
  static void declare(const std::string &class_name,
                      PrimaryGeneratorBuilder *builder);

  /**
   * Generate a primary vertex.
   *
   * This is a pure virtual method and must be defined by a deriving class.
   *
   * @param event The Geant4 event to which the primary vertex will be added.
   */
  virtual void GeneratePrimaryVertex(G4Event *event) = 0;

 protected:
  /// Name of the PrimaryGenerator
  std::string name_{""};

  /// The set of parameters used to configure this class
  fire::config::Parameters params_;

}; // PrimaryGenerator
} // namespace g4fire

/**
 * @macro DECLARE_GENERATOR
 *
 * Defines a builder for the declared class and then registers the class as a
 * generator with the PrimaryGeneratorManager.
 */
#define DECLARE_GENERATOR(NS, CLASS)                                           \
  g4fire::PrimaryGenerator *CLASS##Builder(                                    \
      const std::string &name, fire::config::Parameters &parameters) {         \
    return new NS::CLASS(name, parameters);                                    \
  }                                                                            \
  __attribute((constructor(305))) static void CLASS##Declare() {               \
    g4fire::PrimaryGenerator::declare(                                         \
        std::string(#NS) + "::" + std::string(#CLASS), &CLASS##Builder);       \
  }

#endif // G4FIRE_PRIMARYGENERATOR_H
