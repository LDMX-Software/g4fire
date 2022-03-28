#include "g4fire/Simulator.h"

#include "fire/Process.h"
#include "fire/RandomNumberSeedService.h"
#include "fire/exception/Exception.h"
#include "fire/version/Version.h"

#include "g4fire/darkbrem/G4eDarkBremsstrahlung.h"
#include "g4fire/DetectorConstruction.h"
#include "g4fire/G4Session.h"
#include "g4fire/PluginFactory.h"
#include "g4fire/RunManager.h"
#include "g4fire/geo/ParserFactory.h"

#include "G4CascadeParameters.hh"
#include "G4Electron.hh"
#include "G4GDMLParser.hh"
#include "G4GeometryManager.hh"
#include "G4UIsession.hh"
#include "Randomize.hh"

#include "G4UImanager.hh"

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

  // Instantiate the run manager.
  run_manager_ = std::make_unique<RunManager>(params, conditions_intf_);

  // Instantiate the GDML parser
  auto parser{g4fire::geo::ParserFactory::getInstance().createParser(
      "gdml", params_, conditions_intf_)};

  // Instantiate the class so cascade params can be set.
  G4CascadeParameters::Instance();

  // Set the DetectorConstruction instance used to build the detector
  // from the GDML description.
  run_manager_->SetUserInitialization(
      new DetectorConstruction(parser, params_, conditions_intf_));

  G4GeometryManager::GetInstance()->OpenGeometry();
  parser->read();
  run_manager_->DefineWorldVolume(parser->GetWorldVolume());

  auto pre_init_cmds{
      params_.get<std::vector<std::string>>("pre_init_cmds", {})};
  for (const auto &cmd : pre_init_cmds) {
    if (allowed(cmd)) {
      if (auto g4ret{ui_manager_->ApplyCommand(cmd)}; g4ret > 0) {
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
  auto detector{static_cast<RunManager *>(RunManager::GetRunManager())
                    ->getDetectorConstruction()};

  if (!detector)
    throw fire::Exception("SimSetup",
                          "Detector not constructed before run start.", false);

  // TODO(OM) LDMX specific things should be moved elsewhere.
  header.set<std::string>("Detector Name", detector->getDetectorName());
  header.set<std::string>("Description",
                          params_.get<std::string>("description"));

  header.set<int>("Save calorimeter hit contribs",
                  params_.get<bool>("enable_hit_contribs"));
  header.set<int>("Compress calorimeter hit contribs",
                  params_.get<bool>("compress_hit_contribs"));
  header.set<int>("Included scoring planes",
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
    threeVectorDump("Beam spot delta [mm]", beam_spot_delta);

  // lambda function for dumping vectors of strings to the run header
  auto stringVectorDump = [&header](const std::string &name,
                                    const std::vector<std::string> &vec) {
    int index = 0;
    for (auto const &val : vec) {
      header.set<std::string>(name + " " + std::to_string(++index), val);
    }
  };

  stringVectorDump("Pre init Command",
                   params_.get<std::vector<std::string>>("pre_init_cmds", {}));
  stringVectorDump("Post init Command",
                   params_.get<std::vector<std::string>>("post_init_cmds", {}));

  auto bops{PluginFactory::getInstance().getBiasingOperators()};
  for (const XsecBiasingOperator *bop : bops) {
    bop->RecordConfig(header);
  }

  auto dark_brem{params_.get<fire::config::Parameters>("dark_brem")};
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
  }     // dark brem has been enabled

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

  // Generate and process a Geant4 event.
  n_events_began_++;
  run_manager_->ProcessOneEvent(event.header().number());

  // If a Geant4 event has been aborted, skip the rest of the processing
  // sequence. This will immediately force the simulation to move on to
  // the next event.
  if (run_manager_->GetCurrentEvent()->IsAborted()) {
    run_manager_->TerminateOneEvent(); // clean up event objects
    this->abortEvent();                // get out of processors loop
  } else {
    eb_.writeEvent(run_manager_->GetCurrentEvent(), event);
  }

  // Write all hit objects to the event

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
  run_manager_->TerminateOneEvent();

  return;
}

void Simulator::onProcessStart() {
  // initialize run
  run_manager_->Initialize();

  // Get the extra simulation configuring commands
  auto post_init_cmds{
      params_.get<std::vector<std::string>>("post_init_cmds", {})};
  for (const auto &cmd : post_init_cmds) {
    if (allowed(cmd)) {
      if (auto g4ret{ui_manager_->ApplyCommand(cmd)}; g4ret > 0) {
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
  run_manager_->ConstructScoringWorlds();

  // Initialize the current run
  run_manager_->RunInitialization();

  // Initialize the event processing
  run_manager_->InitializeEventLoop(1);

  return;
}

void Simulator::onFileClose(const std::string &file_name) {
  // End the current run and print out some basic statistics if verbose
  // level > 0.
  run_manager_->TerminateEventLoop();

  // Pass the **real** number of events to the persistency manager
  // persistencyManager_->setNumEvents(n_events_began_, n_events_completed_);

  // Persist any remaining events, call the end of run action and
  // terminate the Geant4 kernel.
  run_manager_->RunTermination();
}

void Simulator::onProcessEnd() {
  std::cout << "[ Simulator ] : "
            << "Started " << n_events_began_ << " events to produce "
            << n_events_completed_ << " events." << std::endl;

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
  run_manager_.reset(nullptr);

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
