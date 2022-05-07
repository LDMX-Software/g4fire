#include "g4fire/XsecBiasingOperator.h"

namespace g4fire::biasing {

/**
 * Bias the Gamma to Mu Pair process
 */
class GammaToMuPair : public XsecBiasingOperator {
public:
  /**
   * Constructor
   *
   * Calls parent constructor and allows
   * accesss to configuration parameters.
   */
  GammaToMuPair(std::string name, const fire::config::Parameters &p);

  /** Destructor */
  ~GammaToMuPair() = default;

  /**
   * @return Method that returns the biasing operation that will be used
   *         to bias the conversion of gammas to muon pairs.
   */
  G4VBiasingOperation *ProposeOccurenceBiasingOperation(
      const G4Track *track,
      const G4BiasingProcessInterface *callingProcess) final override;

  /// Return the process to bias
  virtual std::string getProcessToBias() const { return "GammaToMuPair"; }

  /// Return the particle to bias
  virtual std::string getParticleToBias() const { return "gamma"; }

  /// Return the volume to bias in
  virtual std::string getVolumeToBias() const { return volume_; }

  /**
   * Record the configuration to the run header
   *
   * @param[in,out] header RunHeader to record to
   */
  virtual void RecordConfig(fire::RunHeader &header) const {
    header.set<std::string>("biasing::GammaToMuPair::Volume", volume_);
    header.set<float>("biasing::GammaToMuPair::Factor", factor_);
    header.set<float>("biasing::GammaToMuPair::Threshold", threshold_);
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