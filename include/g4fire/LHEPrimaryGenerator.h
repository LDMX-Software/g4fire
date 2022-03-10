
#include "g4fire/LHEReader.h"
#include "g4fire/PrimaryGenerator.h"

#include "fire/config/Parameters.h" 

class G4Event;

namespace g4fire {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LHEEvent
 */
class LHEPrimaryGenerator : public PrimaryGenerator {
 public:
  /**
   * Class constructor.
   * @param reader The LHE reader with the event data.
   */
  LHEPrimaryGenerator(const std::string& name,
                      fire::config::Parameters& params);

  /**
   * Class destructor.
   */
  virtual ~LHEPrimaryGenerator();

  /**
   * Generate vertices in the Geant4 event.
   * @param anEvent The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* anEvent);

 private:
  /**
   * The LHE reader with the event data.
   */
  LHEReader* reader_;
};

}  // namespace g4fire
