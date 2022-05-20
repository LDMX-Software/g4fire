#ifndef G4FIRE_PARALLELWORLD_H_
#define G4FIRE_PARALLELWORLD_H_

#include <string>

#include "G4GDMLParser.hh"
#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

#include "g4fire/geo/AuxInfoReader.h"

namespace g4fire {

class ParallelWorld : public G4VUserParallelWorld {
 public:
  /** Constructor */
  ParallelWorld(G4GDMLParser* parser, G4String world_name,
                ConditionsInterface& ci);

  /** Destructor */
  ~ParallelWorld();

  /** */
  void Construct();

  /** */
  void ConstructSD();

 private:
  /// GDML parser
  G4GDMLParser* parser_{nullptr};

  /// The auxiliary GDML info reader. 
  g4fire::geo::AuxInfoReader* aux_info_reader_{nullptr};
};  // ParallelWorld
}  // namespace g4fire

#endif  // G4FIRE_PARALLELWORLD_H_
