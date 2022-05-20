
#include "g4fire/biasing/DarkBrem.h"

#include "G4RunManager.hh"

namespace g4fire {
namespace biasing {

DarkBrem::DarkBrem(std::string name, const fire::config::Parameters& p)
    : XsecBiasingOperator(name, p) {
  volume_ = p.get<std::string>("volume");
  factor_ = p.get<double>("factor");
  bias_all_ = p.get<bool>("bias_all");
}

G4VBiasingOperation* DarkBrem::ProposeOccurenceBiasingOperation(
    const G4Track* track, const G4BiasingProcessInterface* callingProcess) {
  std::string currentProcess =
      callingProcess->GetWrappedProcess()->GetProcessName();
  if (currentProcess.compare(this->getProcessToBias()) == 0) {
    // bias only the primary particle if we don't want to bias all particles
    if (not bias_all_ and track->GetParentID() != 0) return 0;

    G4double interactionLength =
        callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();

    double dbXsecUnbiased = 1. / interactionLength;
    double dbXsecBiased = dbXsecUnbiased * factor_;

    if (G4RunManager::GetRunManager()->GetVerboseLevel() > 1) {
      std::cout << "[ DarkBremXsecBiasingOperator ]: "
                << " Unbiased DBrem xsec: " << dbXsecUnbiased
                << " -> Biased xsec: " << dbXsecBiased << std::endl;
    }

    return BiasedXsec(dbXsecBiased);
  } else
    return 0;
}

void DarkBrem::RecordConfig(fire::RunHeader& h) const {
  h.set<int>("biasing::DarkBrem::Bias All Electrons", bias_all_);
  h.set<float>("biasing::DarkBrem::Factor", factor_);
  h.set<std::string>("biasing::DarkBrem::Volume", volume_);
}

}  // namespace biasing
}  // namespace g4fire

DECLARE_XSECBIASINGOPERATOR(g4fire::biasing, DarkBrem)
