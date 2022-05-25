#ifndef G4FIRE_SIMULATOR_H
#define G4FIRE_SIMULATOR_H

#include <any>
#include <map>
#include <memory>
#include <string>

#include <fire/Processor.h>
#include <fire/config/Parameters.h>

#include <G4RunManager.hh>

#include "g4fire/ConditionsInterface.h"
#include "g4fire/user/PrimaryGenerator.h"
#include "g4fire/user/SensitiveDetector.h"
#include "g4fire/user/Action.h"
#include "g4fire/user/BiasingOperator.h"

#include "g4user/PrimaryGeneratorAction.h"
#include "g4user/SteppingAction.h"
#include "g4user/EventAction.h"
#include "g4user/RunAction.h"
#include "g4user/StackingAction.h"
#include "g4user/TrackingAction.h"

class G4UIsession;
class G4UImanager;

namespace g4fire {

/**
 * Geant4 simulation wrapped within a fire producer.
 */
class Simulator : public fire::Processor, public G4RunManager {
public:
  /*
   * Configure the simulation given the set of parameters passed by the user
   * at runtime.
   *
   * This method is used to organize the configuration of this class using
   * the parameters specified in the python configuration passed to fire.
   * This method will only be called within the constructor and is not
   * exposed publicly.
   *
   * @param[in] params The parameters passed to this class via the python
   *  config.
   */
  Simulator(const fire::config::Parameters &params);

  /// Default destructor
  ~Simulator() = default;

  void process(fire::Event &event) final override;

  /**
   * Given a non-const reference to the new RunHeader,
   * we can add parameters from the simulation here
   * before the run starts.
   *
   * @param header of new run
   */
  void beforeNewRun(fire::RunHeader &header) final override;

  /**
   * Before the run starts (but after the conditions are configured)
   * set up the random seeds for this run.
   *
   * @param[in] header RunHeader for this run, unused
   */
  void onNewRun(const fire::RunHeader &header) final override;

  /**
   * Initialization of simulation
   *
   * This uses the parameters set in the configure method to
   * construct and initialize the simulation objects.
   *
   * This function runs the post init setup commands.
   */
  void onProcessStart() final override;

  /// Callback called once processing is complete.
  void onProcessEnd() final override;

 private:
  /**
   * Check if the input command is allowed to be run.
   *
   * Looks for sub-strings matching the ones listed as an invalid command.
   * These invalid commands are mostly commands where control has been handed
   * over to Simulator.
   */
  bool allowed(const std::string &command) const;

  /**
   * Set the seeds to be used by the Geant4 random engine.
   *
   * @param[in] seeds A vector of seeds to pass to the G4 random
   *      engine.  The vector must contain at least 2 seeds otherwise
   *      an exception is thrown.
   */
  void setSeeds(std::vector<int> seeds);

  /// User interface handle
  G4UImanager *ui_manager_{nullptr};

  /// Handle to the G4Session -> how to deal with G4cout and G4cerr
  std::unique_ptr<G4UIsession> session_handle_;

  /// Commands not allowed to be passed from python config file
  ///     This is because Simulator already runs them.
  static const std::vector<std::string> invalid_cmds;

  /// Number of events started
  int n_events_began_{0};

  /// Number of events completed
  int n_events_completed_{0};

  ///  Conditions interface
  ConditionsInterface conditions_intf_;

  /// the other g4user actions
  g4user::SteppingAction* stepping_action_;
  g4user::EventAction* event_action_;
  g4user::RunAction* run_action_;
  g4user::StackingAction* stacking_action_;
  g4user::TrackingAction* tracking_action_;

  /*********************************************************
   * Python Configuration Parameters
   *********************************************************/

  /// Vebosity for the simulation
  int verbosity_{1};

  /// post-init commands
  std::vector<std::string> post_init_cmds_;

  /// pre-init commands
  std::vector<std::string> pre_init_cmds_;

  /// the sensitive detectors
  std::vector<std::unique_ptr<user::SensitiveDetector>> sensitive_detectors_;

  /// the reference physics list
  std::string ref_phys_list_;

  /// the biasing operators
  std::vector<std::unique_ptr<user::BiasingOperator>> biasing_operators_;

  /// our user actions
  std::vector<std::unique_ptr<user::Action>> user_actions_;

  /// our primary generators
  std::vector<std::unique_ptr<user::PrimaryGenerator>> primary_generators_;

  /// the configuration of physics constructors
  std::vector<fire::config::Parameters> additional_phys_cfg_;

  /// the configuration of biasing operators
  std::vector<fire::config::Parameters> biasing_operators_cfg_;

}; // Simulator
} // namespace g4fire
#endif // G4FIRE_SIMULATOR_H
