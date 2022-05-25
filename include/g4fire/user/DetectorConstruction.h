#ifndef G4FIRE_USER_DETECTORCONSTRUCTION_H
#define G4FIRE_USER_DETECTORCONSTRUCTION_H

#include <fire/factory/Factory.h>
#include <G4VUserDetectorConstruction.hh>

namespace g4fire::user {

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  using Factory = ::fire::factory::Factory<DetectorConstruction,DetectorConstruction*,const fire::config::Parameters&>;
 public:
  DetectorConstruction(const fire::config::Parameters& p)
    : G4VUserDetectorConstruction() {}
  ~DetectorConstruction() = default;
  virtual void RecordConfig(fire::RunHeader& rh) const = 0;
  virtual G4VPhysicalVolume* Construct() = 0;
};  // DetectorConstruction

}  // namespace g4fire::user

#define DECLARE_DETECTOR_CONSTRUCTION(CLASS) \
  namespace { \
  auto v = ::g4fire::user::DetectorConstruction::Factory::get().declare<CLASS>(); \
  }

#endif

