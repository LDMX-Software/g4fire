#include "g4fire/XsecBiasingOperator.h"

namespace g4fire::biasing {

/**
 * Bias the neutron inelastic collsions
 */
class NeutronInelastic : public XsecBiasingOperator {
public:
  /**
   * Constructor
   *
   * Calls parent constructor and allows
   * accesss to configuration parameters.
   */
  NeutronInelastic(std::string name, const fire::config::Parameters &p);

  /** Destructor */
  ~NeutronInelastic() = default;

  /**
   * @return Method that returns the biasing operation that will be used
   *         to bias the neutron inelactic hadronic interactions.
   */
  G4VBiasingOperation *ProposeOccurenceBiasingOperation(
      const G4Track *track,
      const G4BiasingProcessInterface *callingProcess) final override;

  /// Return the process to bias
  virtual std::string getProcessToBias() const { return "neutronInelastic"; }

  /// Return the particle to bias
  virtual std::string getParticleToBias() const { return "neutron"; }

  /// Return the volume to bias in
  virtual std::string getVolumeToBias() const { return volume_; }

  /**
   * Record the configuration to the run header
   *
   * @param[in,out] header RunHeader to record to
   */
  virtual void RecordConfig(fire::RunHeader &header) const {
    header.set<std::string>("biasing::NeutronInelastic::Volume", volume_);
    header.set<float>("biasing::NeutronInelastic::Factor", factor_);
    header.set<float>("biasing::NeutronInelastic::Threshold", threshold_);
  }

private:
  /// The volume to bias in
  std::string volume_;

  /// The biasing factor
  double factor_;

  /// Minimum kinetic energy [MeV] to allow a track to be biased
  double threshold_;
};
} // namespace g4fire::biasing
