#include "Simulator.h"

#include <fire/Process.h>
#include <fire/RandomNumberSeedService.h>
#include <fire/exception/Exception.h>
#include <fire/version/Version.h>

#include <FTFP_BERT.hh>
#include <G4GDMLParser.hh>
#include <G4GenericBiasingPhysics.hh>
#include <G4ParallelWorldPhysics.hh>
#include <G4ProcessTable.hh>
#include <G4VModularPhysicsList.hh>
#include <G4CascadeParameters.hh>
#include <G4Electron.hh>
#include <G4GDMLParser.hh>
#include <G4GeometryManager.hh>
#include <G4UIsession.hh>
#include <G4PhysListFactory.hh>
#include <Randomize.hh>
#include <G4UImanager.hh>

#include "G4Session.h"

#include "g4fire/user/DetectorConstruction.h"
#include "g4fire/user/PhysicsConstructor.h"

namespace g4fire {

const std::vector<std::string> Simulator::invalid_cmds = {
    "/run/initialize",       // hard coded at the right time
    "/run/beamOn",           // passed commands should only be sim setup
    "/random/setSeeds",      // handled by own config parameter (if passed)
    "ldmx",                  // all ldmx messengers have been removed
    "/persistency/gdml/read" // detector description is read after passed a
                             // path to the detector description (required)
};

Simulator::Simulator(const fire::config::Parameters &params)
    : fire::Processor(params), G4RunManager(), conditions_intf_(this) {
  // The UI manager pointer is handled by Geant4
  ui_manager_ = G4UImanager::GetUIpointer();

  // Set the verbosity level.  The default level  is 0.
  verbosity_ = params.get<int>("verbosity", 0);

  // If the verbosity level is set to 0,
  // If the verbosity level is > 1, log everything to a file. Otherwise,
  // dump the output. If a prefix has been specified, append it ot the
  // log message.
  auto logging_prefix{params.get<std::string>("logging_prefix", "g4fire_log")};
  if (verbosity_ == 0)
    session_handle_ = std::make_unique<BatchSession>();
  else if (verbosity_ > 1) {
    if (logging_prefix.empty())
      session_handle_ = std::make_unique<LoggedSession>();
    else
      session_handle_ = std::make_unique<LoggedSession>(
          logging_prefix + "_G4cout.log", logging_prefix + "_G4cerr.log");
  }
  if (session_handle_ != nullptr)
    ui_manager_->SetCoutDestination(session_handle_.get());

  // Instantiate the class so cascade params can be set.
  G4CascadeParameters::Instance();

  // Set the DetectorConstruction instance used to build the detector
  // from the GDML description.
  auto det{params.get<fire::config::Parameters>("detector")};
  this->SetUserInitialization(
      user::DetectorConstruction::Factory::get().make(
        det.get<std::string>("class_name"), det));

  auto sds{params.get<std::vector<fire::config::Parameters>>("sensitive_detectors")};
  for (const auto& sd : sds) {
    sensitive_detectors_.emplace_back(
        user::SensitiveDetector::Factory::get().make(sd.get<std::string>("class_name"), 
          conditions_intf_, sd)
        );
    for (G4LogicalVolume* volume : *G4LogicalVolumeStore::GetInstance()) {
      if (sensitive_detectors_.back()->isSensDet(volume)) {
        // log message?
        volume->SetSensitiveDetector(sensitive_detectors_.back().get());
      }
    }
  }

  auto pre_init_cmds =
      params.get<std::vector<std::string>>("pre_init_cmds", {});
  for (const std::string &cmd : pre_init_cmds) {
    if (allowed(cmd)) {
      int g4ret = ui_manager_->ApplyCommand(cmd);
      if (g4ret > 0) {
        throw fire::Exception("PreInitCmd",
                              "Pre Initialization command '" + cmd +
                                  "' returned a failue status from Geant4: " +
                                  std::to_string(g4ret),
                              false);
      }
      pre_init_cmds_.push_back(cmd);
    } else {
      throw fire::Exception(
          "PreInitCmd",
          "Pre Initialization command '" + cmd +
              "' is not allowed because another part of Simulator handles it.",
          false);
    }
  }

  auto post_init_cmds = params.get<std::vector<std::string>>("post_init_cmds",{});
  for (const std::string& cmd : post_init_cmds) {
    if (not allowed(cmd)) {
      throw fire::Exception("PostInitCmd",
          "Post Initializatino command '"+cmd+
          "' is not allowed because another part of Simulator handles it.", false);
    }
    post_init_cmds_.push_back(cmd);
  }

  ref_phys_list_ = params.get<std::string>("reference_phys_list");

  auto generators{
      params.get<std::vector<fire::config::Parameters> >(
          "generators", {})};
  if (generators.empty()) {
    throw fire::Exception("MissingGenerator",
                    "Need to define some generator of primaries.", false);
  }

  for (auto& generator : generators) {
    primary_generators_.emplace_back(
        user::PrimaryGenerator::Factory::get().make(
          generator.get<std::string>("class_name"), generator)
        );
  }

  stepping_action_ = new g4user::SteppingAction;
  event_action_ = new g4user::EventAction;
  stacking_action_ = new g4user::StackingAction;
  tracking_action_ = new g4user::TrackingAction;
  run_action_ = new g4user::RunAction;

  // Create all user actions and attach them to our G4 User Actions
  auto uas{params.get<std::vector<fire::config::Parameters>>("actions", {})};
  for (auto &ua : uas) {
    auto user_action = user::Action::Factory::get().make(ua.get<std::string>("class_name"), ua);
    for (const user::TYPE& t: user_action->getTypes()) {
      if (t == user::TYPE::STEPPING) stepping_action_->attach(user_action.get());
      else if (t == user::TYPE::EVENT) event_action_->attach(user_action.get());
      else if (t == user::TYPE::STACKING) stacking_action_->attach(user_action.get());
      else if (t == user::TYPE::TRACKING) tracking_action_->attach(user_action.get());
      else if (t == user::TYPE::RUN) run_action_->attach(user_action.get());
    }
    user_actions_.emplace_back(std::move(user_action));
  }

  additional_phys_cfg_ = params.get<std::vector<fire::config::Parameters>>("additional_phys",{});
  biasing_operators_cfg_ = params.get<std::vector<fire::config::Parameters>>("biasing_operators",{});
}

void Simulator::beforeNewRun(fire::RunHeader &header) {
  // Get the detector header from the user detector construction
  static_cast<user::DetectorConstruction*>(this->userDetector)->RecordConfig(header);
  for (const auto& sd : sensitive_detectors_) sd->RecordConfig(header);

  // lambda function for dumping vectors of strings to the run header
  auto stringVectorDump = [&header](const std::string &name,
                                    const std::vector<std::string> &vec) {
    int index = 0;
    for (auto const &val : vec) {
      header.set<std::string>(name + " " + std::to_string(++index), val);
    }
  };

  stringVectorDump("Pre Init Command", pre_init_cmds_);
  stringVectorDump("Post Init Command", post_init_cmds_);
  header.set<std::string>("reference phys list", ref_phys_list_);

  // BOPs
  for (const auto& bop : biasing_operators_) bop->RecordConfig(header);

  // Physics

  // Generators
  for (const auto& pg : primary_generators_) pg->RecordConfig(header);

  // User Action
  for (const auto& ua : user_actions_) ua->RecordConfig(header);

  // Set a string parameter with the Geant4 SHA-1.
  if (G4RunManagerKernel::GetRunManagerKernel()) {
    G4String g4_version{
        G4RunManagerKernel::GetRunManagerKernel()->GetVersionString()};
    header.set<std::string>("Geant4 revision", g4_version);
  } else {
    // ldmx_log(warn) << "Unable to access G4 RunManager Kernel. Will not store
    // "
    //                  "G4 Version string.";
  }

  // header.set<std::string>("ldmx-sw revision", GIT_SHA1);
}

void Simulator::onNewRun(const fire::RunHeader &) {
  auto rseed{getCondition<fire::RandomNumberSeedService>(
      fire::RandomNumberSeedService::CONDITIONS_OBJECT_NAME)};
  std::vector<int> seeds;
  seeds.push_back(rseed.getSeed("Simulator[0]"));
  seeds.push_back(rseed.getSeed("Simulator[1]"));
  setSeeds(seeds);
}

void Simulator::process(fire::Event &event) {
  // Generate and process a Geant4 event.
  n_events_began_++;
  this->ProcessOneEvent(event.header().number());

  // If a Geant4 event has been aborted, skip the rest of the processing
  // sequence. This will immediately force the simulation to move on to
  // the next event.
  if (this->GetCurrentEvent()->IsAborted()) {
    this->TerminateOneEvent();  // clean up event objects
    for (auto& sd : sensitive_detectors_) sd->EndOfEvent();                            
    this->abortEvent();         // get out of processors loop
  }

  auto event_info = static_cast<UserEventInformation*>(
      G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
  auto& event_header = event.getEventHeader();
  event_header.setWeight(event_info->get<double>("weight"));

  // Save the state of the random engine to an output string
  std::ostringstream stream;
  G4Random::saveFullState(stream);
  event_header.set("eventSeed", stream.str());

  // Terminate the event by persisting tracks that have passed
  // all storage cuts and giving the event bus to all the SD
  // and UA callbacks for them to store their event information.
  for (auto& ua : user_actions_) ua->store(event);
  for (auto& sd : sensitive_detectors_) {
    sd->saveHits(event);
    sd->EndOfEvent();
  }
  n_events_completed_++;
  this->TerminateOneEvent();

  return; 
}

void Simulator::onProcessStart() {
  std::cout << "on process start" << std::endl;

  auto physics_list{G4PhysListFactory().GetReferencePhysList(ref_phys_list_)};
  for (const auto& phys : additional_phys_cfg_) {
    physics_list->RegisterPhysics(
        user::PhysicsConstructor::Factory::get().make(
          phys.get<std::string>("class_name"), phys)
        );
  }

  if (!biasing_operators_cfg_.empty()) {
    // Create all the biasing operators that will be used.
    for (fire::config::Parameters &bop : biasing_operators_cfg_) {
      biasing_operators_.emplace_back(
        user::BiasingOperator::Factory::get().make(
          bop.get<std::string>("class_name"), bop)
        );
    }

    auto biasing_physics{new G4GenericBiasingPhysics()};

    // Specify which particles are going to be biased. This will put a biasing
    // interface wrapper around *all* processes associated with these
    // particles.
    for (const auto& bop : biasing_operators_) {
      biasing_physics->Bias(bop->getParticleToBias());
    }

    // Register the physics constructor to the physics list:
    physics_list->RegisterPhysics(biasing_physics);
  }
  SetUserInitialization(physics_list);

  // This is where the physics lists are told to construct their particles and
  // their processes. They are constructed in order, so it is important to 
  // register the biasing physics *after* any other processes that need to be
  // able to be biased
  G4RunManager::Initialize();
  std::cout << "done initializing." << std::endl;

  // register our G4 user actions with the run control
  SetUserAction(new g4user::PrimaryGeneratorAction(primary_generators_));
  SetUserAction(stepping_action_);
  SetUserAction(event_action_);
  SetUserAction(stacking_action_);
  SetUserAction(tracking_action_);
  SetUserAction(run_action_);

  // Get the extra simulation configuring commands
  //  we checked that they were allowed in the constructor
  for (const std::string &cmd : post_init_cmds_) {
    int g4ret{ui_manager_->ApplyCommand(cmd)};
    if (g4ret > 0) {
      throw fire::Exception("PostInitCmd",
                            "Post Initialization command '" + cmd +
                                "' returned a failue status from Geant4: " +
                                std::to_string(g4ret),
                            false);
    }
  }

  // Instantiate the scoring worlds including any parallel worlds.
  this->ConstructScoringWorlds();

  // Initialize the current run
  this->RunInitialization();

  // Initialize the event processing
  this->InitializeEventLoop(1);

  return;
}

void Simulator::onProcessEnd() {
  std::cout << "[ Simulator ] : "
            << "Started " << n_events_began_ << " events to produce "
            << n_events_completed_ << " events." << std::endl;

  // End the current run and print out some basic statistics if verbose
  // level > 0.
  this->TerminateEventLoop();

  // Persist any remaining events, call the end of run action and
  // terminate the Geant4 kernel.
  this->RunTermination();

  // Delete Run Manager
  // From Geant4 Basic Example B01:
  //      Job termination
  //      Free the store: user actions, physics list and detector descriptions
  //      are owned and deleted by the run manager, so they should not be
  //      deleted in the main() program
  // This needs to happen here because otherwise, Geant4 objects are deleted
  // twice:
  //  1. When the histogram file is closed (all ROOT objects created during
  //  processing are put there because ROOT)
  //  2. When Simulator is deleted because run_manager_ is a unique_ptr
  //run_manager_.reset(nullptr);

  // Delete the G4UIsession
  // I don't think this needs to happen here, but since we are cleaning up loose
  // ends...
  session_handle_.reset(nullptr);
}

bool Simulator::allowed(const std::string &command) const {
  for (const std::string &invalid_substring : invalid_cmds) {
    if (command.find(invalid_substring) != std::string::npos) {
      // found invalid substring in this command ==> NOT ALLOWED
      return false;
    }
  }
  // checked all invalid commands ==> ALLOWED
  return true;
}

void Simulator::setSeeds(std::vector<int> seeds) {
  // If no seeds have been specified then return immediately.
  if (seeds.empty())
    return;

  // If seeds are specified, make sure that the container has at least
  // two seeds.  If not, throw an exception.
  if (seeds.size() == 1) {
    throw fire::Exception("ConfigurationException",
                          "At least two seeds need to be specified.", false);
  }

  // Create the array of seeds and pass them to G4Random.  Currently,
  // only 100 seeds can be specified at a time.  If less than 100
  // seeds are specified, the remaining slots are set to 0.
  std::vector<long> seed_vec(100, 0);
  for (std::size_t index{0}; index < seeds.size(); ++index)
    seed_vec[index] = static_cast<long>(seeds[index]);

  // Pass the array of seeds to the random engine.
  G4Random::setTheSeeds(&seed_vec[0]);
}
} // namespace g4fire

DECLARE_PROCESSOR(g4fire::Simulator)
