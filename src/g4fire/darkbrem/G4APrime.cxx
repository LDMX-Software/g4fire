#include "g4fire/darkbrem/G4APrime.h"

//#include "Framework/Exception/Exception.h"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "globals.hh"

namespace g4fire {
namespace darkbrem {

G4APrime *G4APrime::the_a_prime = 0;

G4APrime *G4APrime::APrime(G4double the_mass) {
  if (!the_a_prime) {
    //if (the_mass < 0)
    //  EXCEPTION_RAISE("APMass", "APrime doesn't have a mass set!");

    const G4String &name = "A^1";
    G4double mass = the_mass;
    G4double width = 0.;
    G4double charge = 0;
    G4int ispin = 0;
    G4int iparity = 0;
    G4int iconjugation = 0;
    G4int i_isospin = 0;
    G4int i_isospin3 = 0;
    G4int gparity = 0;
    const G4String &p_type = "APrime";
    G4int lepton = 0;
    G4int baryon = 0;
    G4int encoding = 622;   // PDG ID
    G4bool stable = true;   // stable - no decay
    G4double lifetime = -1; // stable - no decay
    G4DecayTable *decaytable = 0;

    the_a_prime =
        new G4APrime(name, mass, width, charge, ispin, iparity, iconjugation,
                     i_isospin, i_isospin3, gparity, p_type, lepton, baryon,
                     encoding, stable, lifetime, decaytable);
  }

  return the_a_prime;
}

} // namespace darkbrem
} // namespace g4fire
