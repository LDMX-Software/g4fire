#pragma once

#include <any>
#include <map>
#include <string>

#include "G4PhysListFactory.hh"
#include "G4RunManager.hh"

#include "fire/config/Parameters.h" 

namespace g4fire {

class ConditionsInterface;
class DetectorConstruction;
//class UserActionManager;
//class APrimeMessenger;

class RunManager : public G4RunManager {
 public:
  /**
   */
  RunManager(const fire::config::Parameters& params, ConditionsInterface& ci);

  /// Destructor
  ~RunManager() = default;

  /**
   * Initialize physics.
   */
  void setupPhysics();

  /**
   * Perform application initialization.
   */
  void Initialize();

  /**
   * Called at the end of each event.
   *
   * Runs parent process G4RunManager::TerminateOneEvent() and
   * resets the activation for the G4eDarkBremsstrahlung process
   * (if dark brem is possible)
   */
  void TerminateOneEvent();

  /**
   * Get the user detector construction cast to a specific type.
   * @return The user detector construction.
   */
  DetectorConstruction* getDetectorConstruction();

  /**
   * Tell RunManager to use the seed from the root file.
   */
  //void setUseRootSeed(bool useIt = true) { useRootSeed_ = useIt; }

  /**
   * Should we use the seed from the root file?
   */
  //bool useRootSeed() { return useRootSeed_; }

 private:
  /// The set of parameters used to configure the RunManager
  fire::config::Parameters params_;

  /**
   * Factory class for instantiating the physics list.
   */
  G4PhysListFactory physics_list_factory_;

  /**
   * Flag indicating whether a parallel world should be
   * registered
   */
  bool pw_enabled_{false};

  /** Path to GDML description of parallel world. */
  std::string parallel_world_path_{""};

  /**
   * Should we use random seed from root file?
   */
  //bool useRootSeed_{false};

  /** ConditionsInterface
   */
  ConditionsInterface& conditions_intf_;

}; // RunManager
}  // namespace g4fire
