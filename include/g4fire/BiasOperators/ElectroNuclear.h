#ifndef SIMCORE_BIASOPERATORS_ELECTRONUCLEAR_H_
#define SIMCORE_BIASOPERATORS_ELECTRONUCLEAR_H_

#include "g4fire/XsecBiasingOperator.h"

namespace g4fire {
namespace biasoperators {

/**
 * Bias the Electron-Nuclear process
 */
class ElectroNuclear : public XsecBiasingOperator {
 public:
  /**
   * Constructor
   *
   * Calls parent constructor and allows
   * accesss to configuration parameters.
   */
  ElectroNuclear(std::string name, const framework::config::Parameters& p);

  /** Destructor */
  ~ElectroNuclear() {}

  /**
   * @return Method that returns the biasing operation that will be used
   *         to bias the occurence of photonuclear events.
   */
  G4VBiasingOperation* ProposeOccurenceBiasingOperation(
      const G4Track* track,
      const G4BiasingProcessInterface* callingProcess) final override;

  /// Return the process to bias
  virtual std::string getProcessToBias() const { return "electronNuclear"; }

  /// Return the particle to bias
  virtual std::string getParticleToBias() const { return "e-"; }

  /// Return the volume to bias in
  virtual std::string getVolumeToBias() const { return volume_; }

  /**
   * Record the configuration to the run header
   *
   * @param[in,out] header RunHeader to record to
   */
  virtual void RecordConfig(ldmx::RunHeader& header) const {
    header.setStringParameter("BiasOperator::ElectroNuclear::Volume", volume_);
    header.setFloatParameter("BiasOperator::ElectroNuclear::Factor", factor_);
    header.setFloatParameter("BiasOperator::ElectroNuclear::Threshold",
                             threshold_);
  }

 private:
  /// The volume to bias in
  std::string volume_;

  /// The biasing factor
  double factor_;

  /// Minimum kinetic energy [MeV] to allow a track to be biased
  double threshold_;

};  // ElectroNuclear

}  // namespace biasoperators
}  // namespace g4fire

#endif  // SIMCORE_BIASOPERATORS_ELECTRONUCLEAR_H_
