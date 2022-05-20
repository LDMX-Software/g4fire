
#include "g4fire/biasing/GammaToMuPair.h"

namespace g4fire::biasing {

GammaToMuPair::GammaToMuPair(std::string name,
                             const fire::config::Parameters& p)
    : XsecBiasingOperator(name, p) {
  volume_ = p.get<std::string>("volume");
  factor_ = p.get<double>("factor");
  threshold_ = p.get<double>("threshold");
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

}  // namespace g4fire

DECLARE_XSECBIASINGOPERATOR(g4fire::biasing, GammaToMuPair)
