#include "g4fire/user/PhysicsConstructor.h"

namespace g4fire::user {
PhysicsConstructor::PhysicsConstructor(const fire::config::Parameters& p)
  : G4VPhysicsConstructor(p.get<std::string>("name")) {}
}
