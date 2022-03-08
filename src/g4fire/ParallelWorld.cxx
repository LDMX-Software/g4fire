
#include "g4fire/ParallelWorld.h"

#include "fire/config/Parameters.h"

namespace g4fire {

ParallelWorld::ParallelWorld(G4GDMLParser *parser, G4String world_name,
                             ConditionsInterface &ci)
    : G4VUserParallelWorld(world_name),
      parser_(parser),
      aux_info_reader_(
          new g4fire::geo::AuxInfoReader(parser, fire::config::Parameters(), ci)) {}

ParallelWorld::~ParallelWorld() { delete aux_info_reader_; }

void ParallelWorld::Construct() {}

void ParallelWorld::ConstructSD() {
  G4VPhysicalVolume *world_physical = GetWorld();
  G4LogicalVolume *world_logical = world_physical->GetLogicalVolume();

  G4LogicalVolume *parallelWorldLogical =
      parser_->GetWorldVolume()->GetLogicalVolume();
  aux_info_reader_->readGlobalAuxInfo();

  for (int index = 0; index < parallelWorldLogical->GetNoDaughters(); index++) {
    G4VPhysicalVolume *physicalVol = parallelWorldLogical->GetDaughter(index);
    std::cout << "[ ParallelWorld ]: Adding : " << physicalVol->GetName()
              << " to parallel world." << std::endl;
    world_logical->AddDaughter(physicalVol);
  }

  aux_info_reader_->assignAuxInfoToVolumes();
}
}  // namespace g4fire
