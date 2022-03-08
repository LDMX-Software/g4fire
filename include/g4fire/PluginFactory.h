#ifndef G4FIRE_PLUGINFACTORY_H
#define G4FIRE_PLUGINFACTORY_H

#include <variant>

#include "fire/config/Parameters.h"

#include "g4fire/PrimaryGenerator.h"
#include "g4fire/USteppingAction.h"
#include "g4fire/UserAction.h"
#include "g4fire/UserEventAction.h"
#include "g4fire/UserRunAction.h"
#include "g4fire/UserStackingAction.h"
#include "g4fire/UserTrackingAction.h"
#include "g4fire/XsecBiasingOperator.h"

/*
#include "Framework/Logger.h"
*/

namespace g4fire {

/**
 * @typedef actionMap
 * A map of the different types of actions to their reference.
 */
typedef std::map<
    TYPE, std::variant<UserRunAction *, UserEventAction *, UserTrackingAction *,
                       USteppingAction *, UserStackingAction *>>
    actionMap;

/**
 * @brief Class that manages the generators used to fire particles.
 *
 * Follows the template for a modern C++ singleton explained
 * <a href="https://stackoverflow.com/a/1008289">on stackoverflow</a>
 */
class PluginFactory {
public:
  /// @return the global PluginFactory instance
  static PluginFactory &getInstance();

  /// Delete the copy constructor
  PluginFactory(const PluginFactory &) = delete;

  /// Delete the assignment operator
  void operator=(const PluginFactory &) = delete;

  /**
   * Get the collection of all enabled generators
   *
   * @return vector of pointers to constructed primary generators
   */
  std::vector<PrimaryGenerator *> getGenerators() const { return generators_; };

  /**
   * Put the primary generator into the list of possible generators
   *
   * @see PrimaryGenerator::declare for where this method is called.
   * This method is used to construct a list of all possible generators that
   * the user could use.
   *
   * @param class_name full name of class (including namespaces) of generator
   * @param builder pointer to function to use to create the generator
   */
  void registerGenerator(const std::string &class_name,
                         PrimaryGeneratorBuilder *builder);

  /**
   * Create a new generate and attach it to the list of generators
   *
   * This checks the list of registered generators for the input class_name.
   * If the class_name is not found, then we assume that the generator is not
   * registered and throw and exception.
   *
   * Otherwise, we use the registered builder to create a generator and give
   * it the passed instance_name and paramters. We insert the created generator
   * into the list of generators.
   *
   * @param class_name Full name of class (including namespaces) of the
   * generator
   * @param instance_name unique run-time instance name for the generator
   * @param params Parameters to pass to the generator for configuration
   */
  void createGenerator(const std::string &class_name,
                       const std::string &instance_name,
                       fire::config::Parameters &params);

  /**
   * Get the map of all types of user actions to
   * a pointer to the user action we have created.
   *
   * @note The createAction method assumes that the internal
   * map of actions has already been created, so the user
   * should call this method before any calls to createAction.
   *
   * @return actionMap of created user actions
   */
  actionMap getActions();

  /**
   * Put the user action into the list of possible actions.
   *
   * @see UserAction::declare for where this method is called.
   * This method is used to construct a list of all possible actions that
   * the user could use.
   *
   * @param class_name full name of class (including namespaces) of action
   * @param builder pointer to function to use to create the action
   */
  void registerAction(const std::string &class_name,
                      UserActionBuilder *builder);

  /**
   * Construct a new action and attach it to the types of actions it will be a
   * part of.
   *
   * This checks the list of registered actions for the input class_name.
   * If the class_name is not found, then we assume that the action is not
   * registered and throw and exception.
   *
   * Otherwise, we use the registered builder to create a action and give
   * it the passed instance_name and paramters. After creation, we then use
   * UserAction::getTypes() to determine which types of actions we should
   * attach this specific action to.
   *
   * @param class_name Full name of class (including namespaces) of the
   * action
   * @param instance_name unique run-time instance name for the action
   * @param params Parameters to pass to the action for configuration
   */
  void createAction(const std::string &class_name,
                    const std::string &instance_name,
                    fire::config::Parameters &params);

  /**
   * Retrieve the current list of biasing operators.
   *
   * In un-biased running modes, this will return an empty vector.
   *
   * @return vector of pointers to biasing operators
   */
  std::vector<XsecBiasingOperator *> getBiasingOperators() const {
    return biasing_operators_;
  }

  /**
   * Put the biasing operator into the list of possible biasing operators.
   *
   * @see XsecBiasingOperator::declare for where this method is called.
   * The declare method is then called using the DECLARE_XSECBIASINGOPERATOR
   * macro.
   *
   * @param class_name Full name of class (including namespaces) of the
   * operator
   * @param builder a pointer to the function that should be used to create
   * the operator
   */
  void registerBiasingOperator(const std::string &class_name,
                               XsecBiasingOperatorBuilder *builder);

  /**
   * Create a biasing operator from the input params.
   *
   * This checks the list of registered biasing operators for the input
   * class_name. If the class_name is not found, then we assume that the biasing
   * operator is not registered and throw and exception.
   *
   * Otherwise, we use the registered builder to create an operator and give
   * it the passed instance_name and parameters. We insert the created operator
   * into the list of biasing operators.
   *
   * @param class_name Full name of class (including namespaces) of the
   * operator
   * @param instance_name unique run-time instance name for the operator
   * @param params Parameters to pass to the operator for configuration
   */
  void createBiasingOperator(const std::string &class_name,
                             const std::string &instance_name,
                             fire::config::Parameters &params);

private:
  /// Constructor - private to prevent initialization
  PluginFactory() {}

  /// A map of all register generators
  std::map<std::string, PrimaryGeneratorBuilder *> registered_generators_;

  /// Cointainer for all generators to be used by the simulation
  std::vector<PrimaryGenerator *> generators_;

  /// A map of all registered user actions to their corresponding info.
  std::map<std::string, UserActionBuilder *> registered_actions_;

  /// Container for all Geant4 actions
  actionMap actions_;

  /// A map of all registered user actions to their corresponding info.
  std::map<std::string, XsecBiasingOperatorBuilder *> registered_operators_;

  /// Container for all biasing operators
  std::vector<XsecBiasingOperator *> biasing_operators_;

  /// Allow for us to log here
  // fire::logging::logger
  // theLog_{fire::logging::makeLogger("SimPluginFactory")};

}; // PluginFactory

} // namespace g4fire

#endif // G4FIRE_PLUGINFACTORY_H
