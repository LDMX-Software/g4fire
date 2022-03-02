
#include "g4fire/BiasOperators/GammaToMuPair.h"

namespace g4fire {
namespace biasoperators {

GammaToMuPair::GammaToMuPair(std::string name,
                             const framework::config::Parameters& p)
    : XsecBiasingOperator(name, p) {
  volume_ = p.getParameter<std::string>("volume");
  factor_ = p.getParameter<double>("factor");
  threshold_ = p.getParameter<double>("threshold");
}

G4VBiasingOperation* GammaToMuPair::ProposeOccurenceBiasingOperation(
    const G4Track* track, const G4BiasingProcessInterface* callingProcess) {
  if (track->GetKineticEnergy() < threshold_) return 0;

  std::string currentProcess =
      callingProcess->GetWrappedProcess()->GetProcessName();
  if (currentProcess.compare(this->getProcessToBias()) == 0) {
    G4double interactionLength =
        callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();

    double enXsecUnbiased = 1. / interactionLength;

    double enXsecBiased = enXsecUnbiased * factor_;

    return BiasedXsec(enXsecBiased);
  } else
    return 0;
}

}  // namespace biasoperators
}  // namespace g4fire

DECLARE_XSECBIASINGOPERATOR(g4fire::biasoperators, GammaToMuPair)
