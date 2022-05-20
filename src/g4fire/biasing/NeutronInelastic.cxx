
#include "g4fire/biasing/NeutronInelastic.h"

namespace g4fire::biasing {

NeutronInelastic::NeutronInelastic(std::string name,
                                   const fire::config::Parameters &p)
    : XsecBiasingOperator(name, p) {
  volume_ = p.get<std::string>("volume");
  factor_ = p.get<double>("factor");
  threshold_ = p.get<double>("threshold");
}

G4VBiasingOperation *NeutronInelastic::ProposeOccurenceBiasingOperation(
    const G4Track *track, const G4BiasingProcessInterface *callingProcess) {

  if (track->GetKineticEnergy() < threshold_)
    return 0;

  std::string currentProcess =
      callingProcess->GetWrappedProcess()->GetProcessName();
  if (currentProcess.compare(this->getProcessToBias()) == 0) {
    G4double interactionLength =
        callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();

    double neutInXsecUnbiased = 1. / interactionLength;

    double neutInXsecBiased = neutInXsecUnbiased * factor_;

    return BiasedXsec(neutInXsecBiased);
  } else
    return 0;
}

} // namespace g4fire::biasing

DECLARE_XSECBIASINGOPERATOR(g4fire::biasing, NeutronInelastic)
