#ifndef G4FIRE_SIMULATOR_H
#define G4FIRE_SIMULATOR_H

#include <any>
#include <map>
#include <memory>
#include <string>

#include "fire/Processor.h"
#include "fire/config/Parameters.h"
#include "fire/config/Parameters.h"
//#include "Framework/EventDef.h"
#include "fire/Processor.h"

#include "g4fire/ConditionsInterface.h"

class G4UImanager;
class G4UIsession;
class G4GDMLParser;
class G4GDMLMessenger;
class G4CascadeParameters;

namespace g4fire {

class RunManager;
class EventFile;
class ParameterSet;
class DetectorConstruction;

/**
 * Geant4 simulation wrapped within a fire producer.
 */
class Simulator : public fire::Processor {
public:
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
   *  Callback for the EventProcessor to take any necessary action
   *  when a new file is opened.
   *
   *  @param eventFile  The input/output file.
   */
  //void onFileOpen(fire::EventFile &eventFile) final override;

  /**
   * Callback for the EventProcessor to take any necessary action
   * when a file is closed.
   *
   * @param eventFile The intput/output file.
   */
  // void onFileClose(fire::EventFile &eventFile) final override;

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

  /// PersistencyManager
  // std::unique_ptr<g4fire::persist::RootPersistencyManager>
  // persistencyManager_;

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

  /*********************************************************
   * Python Configuration Parameters
   *********************************************************/

  /// The parameters used to configure the simulation
  fire::config::Parameters params_;

  /// Vebosity for the simulation
  int verbosity_{1};

}; // Simulator
} // namespace g4fire
#endif // G4FIRE_SIMULATOR_H
