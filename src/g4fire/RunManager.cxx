#include "g4fire/RunManager.h"

#include "FTFP_BERT.hh"
#include "G4GDMLParser.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4ProcessTable.hh"
#include "G4VModularPhysicsList.hh"

#include "g4fire/ConditionsInterface.h"
#include "g4fire/DarkBrem/APrimePhysics.h"
#include "g4fire/DarkBrem/G4eDarkBremsstrahlung.h" //for process name
#include "g4fire/DetectorConstruction.h"
#include "g4fire/GammaPhysics.h"
#include "g4fire/ParallelWorld.h"
#include "g4fire/PluginFactory.h"
#include "g4fire/USteppingAction.h"
#include "g4fire/UserRunAction.h"
#include "g4fire/UserStackingAction.h"
#include "g4fire/UserTrackingAction.h"
/*
#include "g4fire/PrimaryGeneratorAction.h"
#include "g4fire/UserEventAction.h"
*/

namespace g4fire {

RunManager::RunManager(const fire::config::Parameters &params,
                       ConditionsInterface &ci)
    : conditions_intf_(ci) {
  params_ = params;

  // Set whether the ROOT primary generator should use the persisted seed.
  // auto rootPrimaryGenUseSeed{
  //    parameters.getParameter<bool>("rootPrimaryGenUseSeed")};

  // Validate the geometry if specified.
  // setUseRootSeed(rootPrimaryGenUseSeed);
}

void RunManager::setupPhysics() {

  auto physics_list{physics_list_factory_.GetReferencePhysList("FTFP_BERT")};
  physics_list->RegisterPhysics(new GammaPhysics);
  physics_list->RegisterPhysics(new darkbrem::APrimePhysics(
      params.get<fire::config::Parameters>("dark_brem")));

  parallel_world_path_ = params.get<std::string>("scoringPlanes");
  pw_enabled_ = !parallel_world_path_.empty();
  if (pw_enabled_) {
    std::cout
        << "[ RunManager ]: Parallel worlds physics list has been registered."
        << std::endl;
    physics_list->RegisterPhysics(
        new G4ParallelWorldPhysics("ldmxParallelWorld"));
  }

  auto biasing_operators{params.get<std::vector<fire::config::Parameters>>(
      "biasing_operators", {})};
  if (!biasing_operators.empty()) {
    std::cout << "[ RunManager ]: Biasing enabled with "
              << biasing_operators.size() << " operator(s)." << std::endl;

    // create all the biasing operators that will be used
    for (fire::config::Parameters &bop : biasing_operators) {
      g4fire::PluginFactory::getInstance().createBiasingOperator(
          bop.get<std::string>("class_name"),
          bop.get<std::string>("instance_name"), bop);
    }

    // Instantiate the constructor used when biasing
    G4GenericBiasingPhysics *biasing_physics = new G4GenericBiasingPhysics();

    // specify which particles are going to be biased
    //  this will put a biasing interface wrapper around *all* processes
    //  associated with these particles
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
}

void RunManager::Initialize() {
  setupPhysics();

  // The parallel world needs to be registered before the mass world is
  // constructed i.e. before G4RunManager::Initialize() is called.
  (pw_enabled_) {
    std::cout << "[ RunManager ]: Parallel worlds have been enabled."
              << std::endl;

    auto validate_geometry_{params.get<bool>("validate_detector")};
    G4GDMLParser *pw_parser = new G4GDMLParser();
    pw_parser->Read(parallel_world_path_, validate_geometry_);
    this->getDetectorConstruction()->RegisterParallelWorld(
        new ParallelWorld(pw_parser, "ldmxParallelWorld", conditions_intf_));
  }

  // This is where the physics lists are told to construct their particles and
  // their processes
  //  They are constructed in order, so it is important to register the biasing
  //  physics *after* any other processes that need to be able to be biased
  G4RunManager::Initialize();

  // Instantiate the primary generator action
  auto primaryGeneratorAction{new PrimaryGeneratorAction(params)};
  SetUserAction(primaryGeneratorAction);

  // Get instances of all G4 actions
  //      also create them in the factory
  auto actions{PluginFactory::getInstance().getActions()};

  // Create all user actions
  auto userActions{
      params.get<std::vector<g4fire::config::Parameters>>("actions", {})};
  for (auto &userAction : userActions) {
    PluginFactory::getInstance().createAction(
        userAction.get<std::string>("class_name"),
        userAction.get<std::string>("instance_name"), userAction);
  }

  // Register all actions with the G4 engine
  for (const auto &[key, act] : actions) {
    std::visit([this](auto &&arg) { this->SetUserAction(arg); }, act);
  }
}

void RunManager::TerminateOneEvent() {
  // have geant4 do its own thing
  G4RunManager::TerminateOneEvent();

  // reset dark brem process (if needed)
  G4ProcessTable *ptable = G4ProcessTable::GetProcessTable();
  G4int verbosity = ptable->GetVerboseLevel();

  // Only one of these processes should be in the table
  //  (i.e. either the Dark Brem is biased or its not)
  // BUT we want to be able to cover both options without
  // the user having to configure it, so we set both
  // of these processes to active (by passing 'true')
  // while the table is silenced. If the table isn't silenced,
  // the process that isn't in the table will cause the table
  // to throw a "not found" warning.
  std::vector<G4String> dark_brem_processes = {
      darkbrem::G4eDarkBremsstrahlung::PROCESS_NAME,
      "biasWrapper(" + darkbrem::G4eDarkBremsstrahlung::PROCESS_NAME + ")"};
  ptable->SetVerboseLevel(
      0); // silent ptable while searching for process that may/may not exist
  for (auto const &name : dark_brem_processes)
    ptable->SetProcessActivation(name, true);
  ptable->SetVerboseLevel(verbosity);

  if (this->GetVerboseLevel() > 1) {
    std::cout << "[ RunManager ] : "
              << "Reset the dark brem process (if it was activated)."
              << std::endl;
  }
  ptable->SetVerboseLevel(verbosity);
}

DetectorConstruction *RunManager::getDetectorConstruction() {
  return static_cast<DetectorConstruction *>(this->userDetector);
}

} // namespace g4fire
