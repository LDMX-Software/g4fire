#ifndef G4FIRE_DARKBREM_G4APRIME_H
#define G4FIRE_DARKBREM_G4APRIME_H

#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

class G4String;
class G4DecayTable;

namespace g4fire {
namespace darkbrem {

class G4APrime : public G4ParticleDefinition {
private:
  /** Reference to single particle definition of A' */
  static G4APrime *the_a_prime;

  /**
   * Constructor
   *
   * Passes all parameters to the base class constructor
   * to register this particle definition with Geant4.
   */
  G4APrime(const G4String &Name, G4double mass, G4double width, G4double charge,
           G4int ispin, G4int iparity, G4int iconjugation, G4int i_isospin,
           G4int i_isospin3, G4int gparity, const G4String &p_type,
           G4int lepton, G4int baryon, G4int encoding, G4bool stable,
           G4double lifetime, G4DecayTable *decaytable)
      : G4ParticleDefinition(Name, mass, width, charge, ispin, iparity,
                             iconjugation, i_isospin, i_isospin3, gparity,
                             p_type, lepton, baryon, encoding, stable, lifetime,
                             decaytable) {}

  /**
   * Destructor
   *
   * Does nothing on purpose.
   */
  virtual ~G4APrime() {}

public:
  /**
   * Accessor for APrime definition
   *
   * The first call to this function defines the mass
   * and then later calls can call it without an argument
   * because we will just return the single instance
   * of the A' definition.
   *
   * @param[in] the_mass mass of the A' in MeV
   */
  static G4APrime *APrime(G4double the_mass = -1 * MeV);
};

} // namespace darkbrem
} // namespace g4fire

#endif // G4FIRE_DARKBREM_G4APRIME_H_
