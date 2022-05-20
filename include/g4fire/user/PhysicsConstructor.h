#ifndef G4FIRE_USER_PHYSICSCONSTRUCTOR_H
#define G4FIRE_USER_PHYSICSCONSTRUCTOR_H

#include <G4VPhysicsConstructor.hh>

#include <fire/config/Parameters.h>
#include <fire/factory/Factory.h>

namespace g4fire::user {
class PhysicsConstructor : class G4VPhysicsConstructor {
 public:
  using Factory = ::fire::factory::Factory<PhysicsConstructor,PhysicsConstructor*,const fire::config::Parameters&>;
 public:
  PhysicsConstructor(const fire::config::Parameters& p)
    : G4VPhysicsConstructor(p.get<std::string>("name")) {}
  virtual ~PhysicsConstructor() = default;
  virtual void ConstructParticle() = 0;
  virtual void ConstructProcess() = 0;
};
}

#endif

