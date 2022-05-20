#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>

#include "fire/Processor.h"
#include "fire/config/Parameters.h"

#include "g4fire/ConditionsInterface.h"
#include "g4fire/event/EventBuilder.h"

class G4UImanager;
class G4UIsession;

namespace g4fire {

class RunManager;

/**
 * Geant4 simulation wrapped within a fire producer.
 */
class Simulator : public fire::Processor {
 public:
  /**
   * Constructor.
   *
   * @param params The parameters used to configure the simulation.
   */
  Simulator(const fire::config::Parameters &params);

  /// Default destructor
  ~Simulator() = default;

  /**
   * Generate an event by firing the particles of interest through the detector
   * using the generator specified in the configuration.
   *
   * This method is called once per event and initializes the propagation of
   * particles through the detector. All Geant4 tracks (particles) generated in
   * the propagation are stored along with basic information (e.g. momentum,
   * start position). Once all tracks have been fully propagated, the event is
   * passed to the EventBuilder which creates objects to persist and adds them
   * to the event bus.
   *
   * @param[in] event The current event being processed.
   */
  void process(fire::Event &event) final override;

  /**
   * Given a non-const reference to the new RunHeader, we can add parameters
   * from the simulation here before the run starts.
   *
   * @param[in] header The header of the new run.
   */
  void beforeNewRun(fire::RunHeader &header) final override;

  /**
   * Before the run starts (but after the conditions are configured) set up the
   * random seeds for this run.
   *
   * @param[in] header RunHeader for this run, unused
   */
  void onNewRun(const fire::RunHeader &header) final override;

  /**
   * Callback for the Processor to take any necessary action when a event input
   * file is closed.
   *
   * @param[in] file_name The input event file name.
   */
  void onFileClose(const std::string &file_name) final override;

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
  void configure(const fire::config::Parameters &params);

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

  /// Manager controlling G4 simulation run
  std::unique_ptr<RunManager> run_manager_;

  /// User interface handle
  G4UImanager *ui_manager_{nullptr};

  /// The event builder used to create the objects to persist.
  g4fire::event::EventBuilder eb_;

  /// Handle to the G4Session -> how to deal with G4cout and G4cerr
  std::unique_ptr<G4UIsession> session_handle_;

  /// Commands not allowed to be passed from python config file. This is
  /// because Simulator already runs them.
  static const std::vector<std::string> invalid_cmds;

  /// Number of events started
  int n_events_began_{0};

  /// Number of events completed
  int n_events_completed_{0};

  ///  Conditions interface
  ConditionsInterface conditions_intf_;

  /// The parameters used to configure the simulation
  fire::config::Parameters params_;

  /// Vebosity for the simulation
  int verbosity_{1};

}; // Simulator
} // namespace g4fire
