
#include "g4fire/XsecBiasingOperator.h"

namespace g4fire::biasing {

/**
 * Bias the Photon-Nuclear process
 */
class PhotoNuclear : public XsecBiasingOperator {
public:
  /** Constructor */
  PhotoNuclear(std::string name, const fire::config::Parameters &p);

  /** Method called at the beginning of a run. */
  void StartRun();

  /**
   * @return Method that returns the biasing operation that will be used
   *         to bias the occurence of photonuclear events.
   */
  G4VBiasingOperation *ProposeOccurenceBiasingOperation(
      const G4Track *track,
      const G4BiasingProcessInterface *callingProcess) final override;

  /// return the process we want to bias
  virtual std::string getProcessToBias() const { return "photonNuclear"; }

  /// return the particle that we want to bias
  virtual std::string getParticleToBias() const { return "gamma"; }

  /// return the volume we want to bias within
  virtual std::string getVolumeToBias() const { return volume_; }

  /// record the configuration into the run header
  virtual void RecordConfig(fire::RunHeader &h) const {
    h.set<std::string>("biasings::PhotoNuclear::Volume", volume_);
    h.set<float>("biasings::PhotoNuclear::Threshold", threshold_);
    h.set<float>("biasings::PhotoNuclear::Factor", factor_);
    h.set<int>("biasings::PhotoNuclear::Bias Conv Down", down_bias_conv_);
    h.set<int>("biasings::PhotoNuclear::Only Children Of Primary",
               only_children_of_primary_);
  }

private:
  /** Geant4 gamma conversion process name. */
  static const std::string CONVERSION_PROCESS;

  /** Cross-section biasing operation for conversion process */
  G4BOptnChangeCrossSection *emXsecOperation{nullptr};

  /** Unbiased photonuclear xsec. */
  double pnXsecUnbiased_{0};

  /** Biased photonuclear xsec. */
  double pnXsecBiased_{0};

  /** Volume we are going to bias within */
  std::string volume_;

  /** minimum kinetic energy [MeV] for a track to be biased */
  double threshold_;

  /** factor to bias PN by */
  double factor_;

  /// Should we down-bias the gamma conversion process?
  bool down_bias_conv_;

  /// Should we restrict biasing to only children of primary?
  bool only_children_of_primary_;

}; // PhotoNuclear
} // namespace g4fire::biasing