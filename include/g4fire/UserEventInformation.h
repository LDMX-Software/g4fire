#ifndef G4FIRE_USEREVENTINFORMATION_H
#define G4FIRE_USEREVENTINFORMATION_H

#include "G4VUserEventInformation.hh"

namespace g4fire {

/**
 * Encapsulates user defined information associated with a Geant4 event.
 */
class UserEventInformation : public G4VUserEventInformation {
 public:
  /// Constructor
  UserEventInformation() = default;

  /// Destructor
  ~UserEventInformation() = default;

  /// Print the information associated with the track
  // TODO(OM): Use stream operator instead
  void Print() const final override;

  /// Increment the number of brem candidates in an event.
  void incBremCandidateCount() { brem_candidate_count_ += 1; }

  /// Decrease the number of brem candidates in an event.
  void decBremCandidateCount() { brem_candidate_count_ -= 1; }

  /**
   * Set the event weight.
   *
   * @param[in] weight the event weight
   */
  void setWeight(double weight) { weight_ = weight; }

  /**
   * @return The event weight
   */
  double getWeight() { return weight_; }

  /**
   * Increment the event weight by the input weight
   * for an individual step.
   *
   * @param[in] step_weight weight of an individual step
   */
  void incWeight(double step_weight) { weight_ *= step_weight; }

  /**
   * @return The total number of brem candidates that this event
   *      contains.
   */
  int bremCandidateCount() { return brem_candidate_count_; }

  /**
   * Add energy to the photonuclear running total
   *
   * @param[in] delta energy to add to the total
   */
  void addPNEnergy(double delta) { total_photonuclear_energy_ += delta; }

  /**
   * Add energy to the electronuclear running total
   *
   * @param[in] delta energy to add to the total
   */
  void addENEnergy(double delta) { total_electronuclear_energy_ += delta; }

  /**
   * Get the total energy that went PN
   * @returns total simulated energy that went PN in this event
   */
  double getPNEnergy() const { return total_photonuclear_energy_; }

  /**
   * Get the total energy that went EN
   * @returns total simulated energy that went EN in this event
   */
  double getENEnergy() const { return total_electronuclear_energy_; }

  /**
   * Tell us if last step was PN
   * @param[in] yes true if it was
   */
  void lastStepWasPN(bool yes) { last_step_pn_ = yes; }

  /**
   * Was the last step a PN interaction?
   * @returns true if it was
   */
  bool wasLastStepPN() const { return last_step_pn_; }

  /**
   * Tell us if last step was EN
   * @param[in] yes true if it was
   */
  void lastStepWasEN(bool yes) { last_step_en_ = yes; }

  /**
   * Was the last step a EN interaction?
   * @returns true if it was
   */
  bool wasLastStepEN() const { return last_step_en_; }

 private:
  /// Total number of brem candidates in the event
  int brem_candidate_count_{0};

  /**
   * The event weight
   *
   * @note The action WeightByStep relies on the assumption
   * that this weight *starts at 1*, so the value of this
   * member variable should always be 1.
   */
  double weight_{1.};

  /**
   * The total energy that went into the photon-nuclear interaction
   *
   * @note The action TrackNuclearEnergy relies on the assumption
   * that this running total starts at 0, so the value of this
   * member variable should always be 0 at construction.
   */
  double total_photonuclear_energy_{0.};

  /**
   * Was the most recent step a photon-nuclear interaction?
   */
  bool last_step_pn_{false};

  /**
   * The total energy that went into the electron-nuclear interaction
   *
   * @note The action TrackNuclearEnergy relies on the assumption
   * that this running total starts at 0, so the value of this
   * member variable should always be 0 at construction.
   */
  double total_electronuclear_energy_{0.};

  /**
   * Was the most recent step a electron-nuclear interaction?
   */
  bool last_step_en_{false};
};
} // namespace g4fire

#endif // G4FIRE_USEREVENTINFORMATION_H
