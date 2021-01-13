#ifndef SIMCORE_SENSITIVEDETECTOR_H_
#define SIMCORE_SENSITIVEDETECTOR_H_

#include "Framework/Configure/Parameters.h"
#include "Framework/RunHeader.h"

#include "SimCore/ConditionsInterface.h"

//------------//
//   Geant4   //
//------------//
#include "G4VSensitiveDetector.hh"

namespace simcore {

/// Forward declaration for generic building function
class SensitiveDetector;

/// Define type of building fuction for biasing operators
typedef SensitiveDetector* SensitiveDetectorBuilder(
    const std::string& name, 
    simcore::ConditionsInterface& ci,
    const framework::config::Parameters& parameters);

/**
 */
class SensitiveDetector : public G4VSensitiveDetector {
 public:
  /**
   * Constructor
   *
   * @param[in] name unique instance name for this sensitive detector
   * @param[in] ci handle to current conditions interface
   * @param[in] parameters python configuration parameters
   */
  SensitiveDetector(const std::string& name,
                    simcore::ConditionsInterface& ci, 
                    const framework::config::Parameters& parameters);

  /** Destructor */
  virtual ~SensitiveDetector();

  /**
   * Method used to register a detector with the manager.
   *
   * @param className Name of the class instance
   * @param builder The builder used to create and instance of this class.
   */
  static void declare(const std::string& className,
                      SensitiveDetectorBuilder* builder);

  /**
   * Here, we must determine if we should be attached to the 
   * input logical volume. Return 'true' if we should be attached
   * to it and 'false' otherwise.
   * 
   * @param[in] lv logical volume to check
   * @returns true if the input lv should be connected to this sensitive detector
   */
  virtual bool isSensDet(G4LogicalVolume* lv) const = 0;

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
  //virtual void RecordConfig(framework::RunHeader& header) const = 0;

 protected:
  /**
   * Get a condition object from the conditions interface
   *
   * Used in the same way that EventProcessors can retrieve conditions.
   *
   * @tparam[in,out] T type of condition to get
   * @param[in] name name of condition to get
   * @returns condition object requested
   */
  template <class T> const T &getCondition(const std::string &condition_name) {
    return conditions_interface_.getCondition<T>(condition_name);
  }

 private:
  /// Handle to our interface to conditions objects
  simcore::ConditionsInterface& conditions_interface_;

};  // SensitiveDetector
}  // namespace simcore

/**
 * @macro DECLARE_SENSITIVEDETECTOR
 *
 * Defines a builder for the declared class
 * and then registers the class as a possible sensitive detector
 */
#define DECLARE_SENSITIVEDETECTOR(NS, CLASS)                             \
  simcore::SensitiveDetector* CLASS##Builder(                            \
      const std::string& name,                                           \
      simcore::ConditionsInterface& ci,                                  \
      const framework::config::Parameters& parameters) {                 \
    return new NS::CLASS(name, ci, parameters);                          \
  }                                                                      \
  __attribute((constructor(205))) static void CLASS##Declare() {         \
    simcore::SensitiveDetector::declare(                                 \
        std::string(#NS) + "::" + std::string(#CLASS), &CLASS##Builder); \
  }

#endif  // SIMCORE_SENSITIVEDETECTOR_H_