#ifndef G4FIRE_USER_PHYSICSCONSTRUCTOR_H
#define G4FIRE_USER_PHYSICSCONSTRUCTOR_H

#include <G4VPhysicsConstructor.hh>

#include <fire/config/Parameters.h>
#include <fire/factory/Factory.h>
#include <fire/RunHeader.h>

namespace g4fire::user {
class PhysicsConstructor : public G4VPhysicsConstructor {
 public:
  /**
   * returns raw pointer because Geant4 physics list handles
   * clean up
   */
  using Factory = ::fire::factory::Factory<PhysicsConstructor,
        PhysicsConstructor*,const fire::config::Parameters&>;
 public:
  PhysicsConstructor(const fire::config::Parameters& p);
  virtual ~PhysicsConstructor() = default;
  virtual void RecordConfig(fire::RunHeader& rh) const = 0;
  virtual void ConstructParticle() = 0;
  virtual void ConstructProcess() = 0;
};
}

#endif

