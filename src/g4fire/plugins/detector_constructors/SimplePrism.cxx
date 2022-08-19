#include <g4fire/user/DetectorConstruction.h>

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>

namespace g4fire::plugins::detector_constructions {

/**
 * Construct a simple box where there is one material inside
 * and another material outside. 
 *
 * The center of the box is the origin of the world coordinate system. 
 * The materials are taken from G4NistManager.
 * You can find out the list of available materials from the Geant4
 * you have compiled g4fire with by launching a Geant4 interactive
 * terminal and running
 * ```
 * Idle> /material/nist/listMaterials all
 * ```
 * or by compiling a program which just runs
 * ```cpp
 * G4NistManager::Instance()->ListMaterials("all");
 * ```
 */
class SimplePrism : public user::DetectorConstruction {
  std::vector<float> box_dimensions_mm_; 
  std::string box_material_;
  std::string world_material_;
 public:
  SimplePrism(const fire::config::Parameters& p)
    : user::DetectorConstruction(p) {
      box_dimensions_mm_ = p.get<std::vector<float>>("box_dimensions_mm");
      box_material_ = p.get<std::string>("box_material");
      world_material_ = p.get<std::string>("world_material");
  }
  virtual void RecordConfig(fire::RunHeader& rh) const {
    rh.set("SimplePrism X [mm]", box_dimensions_mm_.at(0));
    rh.set("SimplePrism Y [mm]", box_dimensions_mm_.at(1));
    rh.set("SimplePrism Z [mm]", box_dimensions_mm_.at(2));
    rh.set("SimplePrism Box Material", box_material_);
    rh.set("SimplePrism World Material", world_material_);
  }
  virtual G4VPhysicalVolume* Construct() {
    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    using CLHEP::mm;

    G4double box_x{box_dimensions_mm_.at(0)*mm},
             box_y{box_dimensions_mm_.at(1)*mm},
             box_z{box_dimensions_mm_.at(2)*mm};
    G4Material* box_mat = nist->FindOrBuildMaterial(box_material_);
    if (not box_mat) {
      throw fire::Exception("BadMaterial","Material '"+box_material_+"' unkown to G4NistManager.");
    }

    G4double
      world_x(box_x*1.2), world_y(box_y*1.2), world_z(box_z*1.2);
    G4Material* world_mat = nist->FindOrBuildMaterial(world_material_);
    if (not world_mat) {
      throw fire::Exception("BadMaterial","Material '"+box_material_+"' unkown to G4NistManager.");
    }

    G4Box* solidWorld =
      new G4Box("World", 0.5*world_x, 0.5*world_y, 0.5*world_z);

    G4LogicalVolume* logicWorld =
      new G4LogicalVolume(solidWorld,
          world_mat,
          "World");

    G4VPhysicalVolume* physWorld =
      new G4PVPlacement(0, //no rotation
          G4ThreeVector(), //at (0,0,0)
          logicWorld,      //its logical volume
          "World",         //its name
          0,               //its mother  volume
          false,           //no boolean operation
          0,               //copy number
          false);          //overlaps checking

    G4Box* solidBox = new G4Box("Box",
        0.5*box_x, 0.5*box_y, 0.5*box_z);

    G4LogicalVolume* logicBox = new G4LogicalVolume(solidBox,
        box_mat, "Box");

    // providing mother volume attaches us to the world volume
    new G4PVPlacement(0, //no rotation
        G4ThreeVector(), //at (0,0,0)
        logicBox,        //its logical volume
        "Envelope",      //its name
        logicWorld,      //its mother  volume
        false,           //no boolean operation
        0,               //copy number
        false);          //overlaps checking

    //always return the physical World
    return physWorld;
  }
};

}

DECLARE_DETECTOR_CONSTRUCTION(g4fire::plugins::detector_constructions::SimplePrism);
