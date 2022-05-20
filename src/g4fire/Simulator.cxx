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

#include "g4fire/GammaPhysics.h"
#include "g4fire/G4Session.h"
#include "g4fire/ParallelWorld.h"

#include "g4fire/g4user/PrimaryGeneratorAction.h"
#include "g4fire/g4user/SteppingAction.h"
#include "g4fire/g4user/EventAction.h"
#include "g4fire/g4user/RunAction.h"
#include "g4fire/g4user/StackingAction.h"
#include "g4fire/g4user/TrackingAction.h"

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
    : fire::Processor(params), conditions_intf_(this) {
  // The UI manager pointer is handled by Geant4
  ui_manager_ = G4UImanager::GetUIpointer();

  // Configure this processor
  configure(params);
}

void Simulator::configure(const fire::config::Parameters &params) {
  std::cout << "Configuring ..." << std::endl;

  // parameters used to configure the simulation
  params_ = params;

  // Set the verbosity level.  The default level  is 0.
  verbosity_ = params_.get<int>("verbosity", 0);

  // If the verbosity level is set to 0,
  // If the verbosity level is > 1, log everything to a file. Otherwise,
  // dump the output. If a prefix has been specified, append it ot the
  // log message.
  auto logging_prefix{params_.get<std::string>("logging_prefix", "g4fire_log")};
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

  // Instantiate the GDML parser
  auto parser{g4fire::geo::ParserFactory::getInstance().createParser(
      "gdml", params_, conditions_intf_)};

  // Instantiate the class so cascade params can be set.
  G4CascadeParameters::Instance();

  // Set the DetectorConstruction instance used to build the detector
  // from the GDML description.
  auto det{params_.get<fire::config::Parameters>("detector")};
  this->SetUserInitialization(
      user::DetectorConstruction::Factory::get().create(
        det.get<std::string>("class_name"), det));

  auto pre_init_cmds =
      params_.get<std::vector<std::string>>("pre_init_cmds", {});
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
    } else {
      throw fire::Exception(
          "PreInitCmd",
          "Pre Initialization command '" + cmd +
              "' is not allowed because another part of Simulator handles it.",
          false);
    }
  }
}

void Simulator::beforeNewRun(fire::RunHeader &header) {
  // Get the detector header from the user detector construction
  static_cast<user::DetectorConstruction*>(this->userDetector)->RecordConfig(header);

  header.set<int>("Save calorimeter hit contribs",
                  params_.get<bool>("enable_hit_contribs"));
  header.set<int>("Compress calorimeter hit contribs",
                  params_.get<bool>("compress_hit_contribs"));
  header.set<int>("Included Scoring Planes",
                  !params_.get<std::string>("scoring_planes").empty());
  // header.set<int>("Use Random Seed from Event Header",
  //                       params_.get<bool>("rootPrimaryGenUseSeed"));

  // lambda function for dumping 3-vectors into the run header
  auto threeVectorDump = [&header](const std::string &name,
                                   const std::vector<double> &vec) {
    header.set<float>(name + " X", vec.at(0));
    header.set<float>(name + " Y", vec.at(1));
    header.set<float>(name + " Z", vec.at(2));
  };

  auto beam_spot_delta{params_.get<std::vector<double>>("beam_spot_delta", {})};
  if (!beam_spot_delta.empty())
    threeVectorDump("Smear Beam Spot [mm]", beam_spot_delta);

  // lambda function for dumping vectors of strings to the run header
  auto stringVectorDump = [&header](const std::string &name,
                                    const std::vector<std::string> &vec) {
    int index = 0;
    for (auto const &val : vec) {
      header.set<std::string>(name + " " + std::to_string(++index), val);
    }
  };

  stringVectorDump("Pre Init Command",
                   params_.get<std::vector<std::string>>("pre_init_cmds", {}));
  stringVectorDump("Post Init Command",
                   params_.get<std::vector<std::string>>("post_init_cmds", {}));

  auto bops{PluginFactory::getInstance().getBiasingOperators()};
  for (const XsecBiasingOperator *bop : bops) {
    // bop->RecordConfig(header);
  }

  /*auto dark_brem{params_.get<fire::config::Parameters>("dark_brem")};
  if (dark_brem.get<bool>("enable")) {
    // the dark brem process is enabled, find it and then record its
    // configuration
    G4ProcessVector *electron_processes =
        G4Electron::Electron()->GetProcessManager()->GetProcessList();
    int n_electron_processes = electron_processes->size();
    for (int i_process = 0; i_process < n_electron_processes; i_process++) {
      G4VProcess *process = (*electron_processes)[i_process];
      if (process->GetProcessName().contains(
              darkbrem::G4eDarkBremsstrahlung::PROCESS_NAME)) {
        // reset process to wrapped process if it is biased
        if (dynamic_cast<G4BiasingProcessInterface *>(process))
          process = dynamic_cast<G4BiasingProcessInterface *>(process)
                        ->GetWrappedProcess();
        // record the process configuration to the run header
        dynamic_cast<darkbrem::G4eDarkBremsstrahlung *>(process)->RecordConfig(
            header);
        break;
      } // this process is the dark brem process
    }   // loop through electron processes
  } */    // dark brem has been enabled

  auto generators{
      params_.get<std::vector<fire::config::Parameters>>("generators")};
  int counter = 0;
  for (auto const &gen : generators) {
    std::string genID = "Gen " + std::to_string(++counter);
    auto class_name{gen.get<std::string>("class_name")};
    header.set<std::string>(genID + " Class", class_name);

    if (class_name.find("g4fire::ParticleGun") != std::string::npos) {
      header.set<float>(genID + " Time [ns]", gen.get<double>("time"));
      header.set<float>(genID + " Energy [GeV]", gen.get<double>("energy"));
      header.set<std::string>(genID + " Particle",
                              gen.get<std::string>("particle"));
      threeVectorDump(genID + " Position [mm]",
                      gen.get<std::vector<double>>("position"));
      threeVectorDump(genID + " Direction",
                      gen.get<std::vector<double>>("direction"));
    } else if (class_name.find("g4fire::MultiParticleGunPrimaryGenerator") !=
               std::string::npos) {
      header.set<int>(genID + " Poisson Enabled",
                      gen.get<bool>("enablePoisson"));
      header.set<int>(genID + " N Particles", gen.get<int>("nParticles"));
      header.set<int>(genID + " PDG ID", gen.get<int>("pdgID"));
      threeVectorDump(genID + " Vertex [mm]",
                      gen.get<std::vector<double>>("vertex"));
      threeVectorDump(genID + " Momentum [MeV]",
                      gen.get<std::vector<double>>("momentum"));
    } else if (class_name.find("g4fire::LHEPrimaryGenerator") !=
               std::string::npos) {
      header.set<std::string>(genID + " LHE File",
                              gen.get<std::string>("filePath"));
      //}
      // else if (class_name.find("g4fire::RootCompleteReSim") !=
      //           std::string::npos) {
      //  header.set<std::string>(genID + " ROOT File",
      //                            gen.get<std::string>("filePath"));
      //} else if (class_name.find("g4fire::RootSimFromEcalSP") !=
      //           std::string::npos) {
      //  header.set<std::string>(genID + " ROOT File",
      //                            gen.get<std::string>("filePath"));
      //  header.set<float>(genID + " Time Cutoff [ns]",
      //                           gen.get<double>("time_cutoff"));
    } else if (class_name.find("g4fire::GeneralParticleSource") !=
               std::string::npos) {
      stringVectorDump(genID + " Init Cmd",
                       gen.get<std::vector<std::string>>("initCommands"));
    } else {
      // ldmx_log(warn) << "Unrecognized primary generator '" << class_name <<
      //"'. "
      //               << "Will not be saving details to RunHeader.";
    }
  }

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
  std::cout << "Processing." << std::endl;
  // Generate and process a Geant4 event.
  n_events_began_++;
  this->ProcessOneEvent(event.header().number());

  // If a Geant4 event has been aborted, skip the rest of the processing
  // sequence. This will immediately force the simulation to move on to
  // the next event.
  if (this->GetCurrentEvent()->IsAborted()) {
    this->TerminateOneEvent();  // clean up event objects
    this->abortEvent();                // get out of processors loop
  }

  /*if (this->getLogFrequency() > 0 and
      event.getEventHeader().getEventNumber() % this->getLogFrequency() == 0) {
    // print according to log frequency and verbosity
    if (verbosity_ > 1) {
      std::cout << "[ Simulator ] : Printing event contents:" << std::endl;
      event.Print();
    }
  }*/

  // Terminate the event.  This checks if an event is to be stored or
  // stacked for later.
  n_events_completed_++;
  this->TerminateOneEvent();

  return; 
}

void Simulator::onProcessStart() {
  std::cout << "on process start" << std::endl;

  auto ref_phys_list{params_.get<std::string>("reference_phys_list")};
  auto physics_list{G4PhysListFactory().GetReferencePhysList(ref_phys_list)};
  auto additional_phys{params_.get<std::vector<fire::config::Parameters>("additional_physics",{})};
  for (const auto& phys : additional_phys) {
    physics_list->RegisterPhysics(
        user::PhysicsConstructor::Factory::get().create(phys.get<std::string>("class_name"), phys)
        );
  }

  std::string parallel_world_path_ = params_.get<std::string>("parallel_world", {});
  bool pw_enabled_ = !parallel_world_path_.empty();
  if (pw_enabled_) {
    // TODO(OM) Use logger instead.
    std::cout
        << "[ RunManager ]: Parallel worlds physics list has been registered."
        << std::endl;
    physics_list->RegisterPhysics(
        new G4ParallelWorldPhysics("parallel_world"));
  }

  auto biasing_operators{params_.get<std::vector<fire::config::Parameters>>(
      "biasing_operators", {})};
  if (!biasing_operators.empty()) {
    std::cout << "[ RunManager ]: Biasing enabled with "
              << biasing_operators.size() << " operator(s)." << std::endl;

    // Create all the biasing operators that will be used.
    for (fire::config::Parameters &bop : biasing_operators) {
      g4fire::PluginFactory::getInstance().createBiasingOperator(
          bop.get<std::string>("class_name"),
          bop.get<std::string>("instance_name"), bop);
    }

    auto biasing_physics{new G4GenericBiasingPhysics()};

    // Specify which particles are going to be biased. This will put a biasing
    // interface wrapper around *all* processes associated with these
    // particles.
    for (const g4fire::XsecBiasingOperator *bop :
         g4fire::PluginFactory::getInstance().getBiasingOperators()) {
      std::cout << "[ RunManager ]: Biasing operator '" << bop->GetName()
                << "' set to bias " << bop->getParticleToBias() << std::endl;
      biasing_physics->Bias(bop->getParticleToBias());
    }

    // Register the physics constructor to the physics list:
    physics_list->RegisterPhysics(biasing_physics);
  }
  this->SetUserInitialization(physics_list);

  // The parallel world needs to be registered before the mass world is
  // constructed i.e. before G4RunManager::Initialize() is called.
  if (pw_enabled_) {
    std::cout << "[ RunManager ]: Parallel worlds have been enabled."
              << std::endl;

    auto validate_geometry_{params_.get<bool>("validate_detector")};
    auto pw_parser{new G4GDMLParser()};
    pw_parser->Read(parallel_world_path_, validate_geometry_);
    this->userDetector->RegisterParallelWorld(
        new ParallelWorld(pw_parser, "parallel_world", conditions_intf_));
  }

  // This is where the physics lists are told to construct their particles and
  // their processes. They are constructed in order, so it is important to 
  // register the biasing physics *after* any other processes that need to be
  // able to be biased
  G4RunManager::Initialize();
  std::cout << "done initializing." << std::endl;

  // Instantiate the primary generator action
  auto primaryGeneratorAction{new PrimaryGeneratorAction(params_)};
  SetUserAction(primaryGeneratorAction);

  // Get instances of all G4 actions
  //      also create them in the factory
  auto actions{PluginFactory::getInstance().getActions()};

  // Create all user actions
  auto userActions{
      params_.get<std::vector<fire::config::Parameters>>("actions", {})};
  for (auto &userAction : userActions) {
    PluginFactory::getInstance().createAction(
        userAction.get<std::string>("class_name"),
        userAction.get<std::string>("instance_name"), userAction);
  }

  // Register all actions with the G4 engine
  for (const auto &[key, act] : actions) {
    std::visit([this](auto &&arg) { this->SetUserAction(arg); }, act);
  }

  // Get the extra simulation configuring commands
  auto post_init_cmds{
      params_.get<std::vector<std::string>>("post_init_cmds", {})};
  for (const std::string &cmd : post_init_cmds) {
    if (allowed(cmd)) {
      int g4ret{ui_manager_->ApplyCommand(cmd)};
      if (g4ret > 0) {
        throw fire::Exception("PostInitCmd",
                              "Post Initialization command '" + cmd +
                                  "' returned a failue status from Geant4: " +
                                  std::to_string(g4ret),
                              false);
      }
    } else {
      throw fire::Exception(
          "PostInitCmd",
          "Post Initialization command '" + cmd +
              "' is not allowed because another part of Simulator handles it.",
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
