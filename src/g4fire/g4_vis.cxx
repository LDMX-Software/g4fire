#include "G4GDMLParser.hh"
#include "G4GeometryManager.hh"
#include "G4PhysListFactory.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "Framework/Configure/Parameters.h"
#include "Framework/EventProcessor.h"
#include "g4fire/DetectorConstruction.h"
#include "g4fire/Geo/ParserFactory.h"

static void printUsage() {
  std::cout << "usage: g4-vis {detector.gdml}" << std::endl;
  std::cout << "  {detector.gdml} is the geometry description "
               "that you wish to visualize."
            << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printUsage();
    std::cerr << "** Need to be given a single detector description. **"
              << std::endl;
    return 1;
  }

  std::string the_arg{argv[1]};
  if (the_arg == "-h" or the_arg == "--help") {
    // ask for help, let's give it to them.
    printUsage();
    return 0;
  }

  framework::EventProcessor* null_processor{nullptr};
  framework::config::Parameters empty_parameters;
  g4fire::ConditionsInterface empty_interface(null_processor);

  // RunManager
  G4RunManager* runManager = new G4RunManager;

  // Detector components
  auto parser{g4fire::geo::ParserFactory::getInstance().createParser(
      "gdml", empty_parameters, empty_interface)};
  runManager->SetUserInitialization(new g4fire::DetectorConstruction(
      parser, empty_parameters, empty_interface));
  G4GeometryManager::GetInstance()->OpenGeometry();
  parser->read();
  runManager->DefineWorldVolume(parser->GetWorldVolume());

  // required to define a physics list to complete initialization
  G4PhysListFactory lists;
  runManager->SetUserInitialization(lists.GetReferencePhysList("FTFP_BERT"));

  runManager->Initialize();

  // Define (G)UI
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  ui->SessionStart();

  delete ui;
  delete runManager;
  delete visManager;

  return 0;
}
