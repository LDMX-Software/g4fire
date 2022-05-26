#ifndef G4FIRE_USER_BIASINGOPERATOR_H_
#define G4FIRE_USER_BIASINGOPERATOR_H_

#include <fire/config/Parameters.h>
#include <fire/factory/Factory.h>
#include <fire/RunHeader.h>

#include <G4BOptnChangeCrossSection.hh>
#include <G4BiasingProcessInterface.hh>
#include <G4Track.hh>
#include <G4VBiasingOperator.hh>

namespace g4fire::user {

/**
 * Our specialization of the biasing operator used with Geant4.
 *
 * This specialization accomplishes three main tasks.
 * 1. Allows any derived class to be dynamically loaded after
 *    using the declaration macro given below.
 * 2. Interfaces with the derived class using our parameters
 *    class.
 * 3. Pre-defines the necessary biasing operation so the derived
 *    class only needs to worry about calculating the biased
 *    xsec.
 */
class BiasingOperator : public G4VBiasingOperator {
 public:
  /**
   * Factory for biasing operators
   */
  using Factory = ::fire::factory::Factory<BiasingOperator,
        std::unique_ptr<BiasingOperator>,const fire::config::Parameters&>;
 public:
  /**
   * Constructor
   *
   * Here, we define a unique name for this biasing operator
   * and are given the configuration params loaded from the
   * python script.
   *
   * @note We require the parameter 'name' defined.
   *
   * @param params python configuration parameters
   */
  BiasingOperator(const fire::config::Parameters& params);

  /// Destructor 
  virtual ~BiasingOperator() = default;

  /**
   * Method called at the beginning of a run.
   *
   * This makes sure that the process we want to bias can
   * be biased and constructs a corresponding biasing operation.
   *
   * It can be over-written, but then the derived class should
   * call `BiasingOperator::StartRun()` at the beginning of
   * their own StartRun.
   */
  virtual void StartRun();

  /**
   * Attach ourselves to the input logical volume if it should be biased
   *
   * Use the Geant4 method G4VBiasingOperator::Attach when you know that lv
   * should be biased.
   *
   * ```cpp
   * this->Attach(lv);
   * ```
   */
  virtual void isBiased(G4LogicalVolume* lv) const = 0;

  /**
   * Propose a biasing operation for the current track and calling process.
   *
   * @note Returning `0` from this function will mean that the current track
   * and process will not be biased.
   *
   * @see BiasedXsec for a method that allows the derived class to not
   * interact with the biasing operation itself.
   *
   * @param track handle to current track that could be biased
   * @param calling_process handle to process asking if it should be biased
   * @return the biasing operation with the biased xsec
   */
  virtual G4VBiasingOperation* ProposeOccurenceBiasingOperation(
      const G4Track* track,
      const G4BiasingProcessInterface* calling_process) = 0;


  /**
   * Return the process whose cross-section will be biased.
   *
   * We need this to be able to check that the process
   * was biased before creating the biasing operator.
   */
  virtual std::string getProcessToBias() const = 0;

  /**
   * Return the particle which should be biased.
   *
   * We need this to be able to tell the physics
   * list which particle to bias.
   * @see RunManager::setupPhysics
   */
  virtual std::string getParticleToBias() const = 0;

  /**
   * Return the volume which should be biased.
   *
   * We need this to be able to tell the detector
   * construction which volumes to attach this
   * operator to.
   */
  virtual std::string getVolumeToBias() const = 0;

  /**
   * Record the configuration of this
   * biasing operator into the run header.
   *
   * @param[in,out] header RunHeader to write configuration to
   */
  virtual void RecordConfig(fire::RunHeader& header) const = 0;

 protected:
  /**
   * Helper method for passing a biased interaction length
   * to the Geant4 biasing fire.
   *
   * Use like:
   *
   *    return BiasedXsec(biased_xsec);
   *
   * inside of ProposeOccurenceBiasingOperation when
   * you want to update the biased cross section.
   *
   * @param biased_xsec the biased cross section
   * @return the biasing operation with the input biased cross section
   */
  G4VBiasingOperation* BiasedXsec(double biased_xsec);

  /**
   * Check if the given processed is being biased.
   *
   * @param process Process of interest
   * @return true if the process is being biased, false otherwise
   */
  bool processIsBiased(const std::string& process);

  /// Cross-section biasing operation.
  G4BOptnChangeCrossSection* xsec_operation_{nullptr};

  /// Process manager associated with the particle of interest. 
  G4ProcessManager* process_manager_{nullptr};

  /// Do *not* propose any biasing on final states.
  G4VBiasingOperation* ProposeFinalStateBiasingOperation(
      const G4Track*, const G4BiasingProcessInterface*) {
    return nullptr;
  }

  /// Do *not* propose any non-physics biasing.
  G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(
      const G4Track*, const G4BiasingProcessInterface*) {
    return nullptr;
  }

}; // BiasingOperator
}  // namespace g4fire::user

/**
 * @macro DECLARE_BIASING_OPERATOR
 *
 * Registers the class as a biasing operator that is available
 */
#define DECLARE_BIASING_OPERATOR(CLASS)                              \
  namespace { \
  auto v = ::g4fire::user::BiasingOperator::Factory::get().declare<CLASS>(); \
  }

#endif  // G4FIRE_USER_BIASINGOPERATOR_H_
