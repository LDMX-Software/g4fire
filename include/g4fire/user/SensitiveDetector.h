#ifndef G4FIRE_USER_SENSITIVEDETECTOR_H_
#define G4FIRE_USER_SENSITIVEDETECTOR_H_

#include <fire/config/Parameteres.h>
#include <fire/RunHeader.h>

#include <G4VSensitiveDetector.hh>

#include "g4fire/ConditionsInterface.h"
/*
#include "SimCore/G4User/TrackingAction.h"
#include "SimCore/TrackMap.h"
*/

namespace g4fire::user {

/**
 * A user class that is called when particles pass through specific volumes
 */
class SensitiveDetector : public G4VSensitiveDetector {
 public:
  /**
   * Factory for sensitive detectors
   */
  using Factory = ::fire::factory::Factory<SensitiveDetector,
        std::unique_ptr<SensitiveDetector>,
        g4fire::ConditionsInterface&, const fire::config::Parameters&>;
 public:
  /**
   * Configure our sensitive detector
   *
   * @param[in] ci handle to current conditions interface
   * @param[in] parameters python configuration parameters
   */
  SensitiveDetector(g4fire::ConditionsInterface& ci, 
                    const fire::config::Parameters& parameters);

  /** Destructor */
  virtual ~SensitiveDetector() = default;

  /**
   * Here, we must determine if we should be attached to the 
   * input logical volume. Use G4LogicalVolume::SetSensitiveDetector
   * if the input lv is this sensitive detecotr
   *
   * ```cpp
   * lv->SetSensitiveDetector(this);
   * ```
   * 
   * @param[in] lv logical volume to check
   */
  virtual void isSensDet(G4LogicalVolume* lv) const = 0;

  /**
   * This is Geant4's handle to tell us that a particle has stepped
   * through our sensitive detector and we should process its interaction with us.
   *
   * @param[in] step the step that happened within one of our logical volumes
   * @param[in] hist the touchable history of the step
   */
  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* hist) = 0;

  /**
   * We are given the event bus here and we must decide
   * now what to persist into the event.
   *
   * @param[in,out] event event bus to add thing(s) to
   */
  virtual void saveHits(framework::Event& event) = 0;

  /**
   * Record the configuration of this
   * detector into the run header.
   *
   * @param[in,out] header RunHeader to write configuration to
   */
  virtual void RecordConfig(framework::RunHeader& header) const = 0;

 protected:
  /**
   * Get a condition object from the conditions interface
   *
   * Used in the same way that Processors can retrieve conditions.
   *
   * @tparam[in,out] T type of condition to get
   * @param[in] name name of condition to get
   * @returns condition object requested
   */
  template <class T> const T &getCondition(const std::string &condition_name) {
    return conditions_interface_.getCondition<T>(condition_name);
  }

  /**
   * Check if the passed step is a step of a geantino
   *
   * @param[in] step Current step to check
   * @returns true if step is coming from neutral/charged genatino
   */
  bool isGeantino(const G4Step* step) const;

  /**
   * Get a handle to the current track map.
   *
   * The track map is created only after the event
   * begins, so you will get a segmentation violation
   * if you call this function before an event has started.
  const TrackMap& getTrackMap() const {
    return *(g4fire::user::TrackingAction::getUserTrackingAction()->getTrackMap());
  }
   */

 private:
  /// Handle to our interface to conditions objects
  simcore::ConditionsInterface& conditions_interface_;

};  // SensitiveDetector
}  // namespace g4fire::user

/**
 * @macro DECLARE_SENSITIVE_DETECTOR
 *
 * Registers the input sensitive detector with the SD factory
 */
#define DECLARE_SENSITIVE_DETECTOR(CLASS) \
  namespace { \
  auto v = ::g4fire::user::SensitiveDetector::Factory::get().declare<CLASS>(); \
  }

#endif  // G4FIRE_USER_SENSITIVEDETECTOR_H_
