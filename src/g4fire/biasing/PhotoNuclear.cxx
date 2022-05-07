
#include "g4fire/biasing/PhotoNuclear.h"

#include "fire/exception/Exception.h"

namespace g4fire::biasing {

const std::string PhotoNuclear::CONVERSION_PROCESS = "conv";

PhotoNuclear::PhotoNuclear(std::string name, const fire::config::Parameters &p)
    : XsecBiasingOperator(name, p) {
  volume_ = p.get<std::string>("volume");
  threshold_ = p.get<double>("threshold");
  factor_ = p.get<double>("factor");
  down_bias_conv_ = p.get<bool>("down_bias_conv");
  only_children_of_primary_ = p.get<bool>("only_children_of_primary");
}

void PhotoNuclear::StartRun() {
  XsecBiasingOperator::StartRun();

  if (processIsBiased(CONVERSION_PROCESS)) {
    emXsecOperation = new G4BOptnChangeCrossSection("changeXsec-conv");
  } else if (down_bias_conv_) {
    throw fire::Exception("PhotoNuclearBiasing",
                          "Gamma Conversion process '" + CONVERSION_PROCESS +
                              "' is not biased!",
                          false);
  }
}

G4VBiasingOperation *PhotoNuclear::ProposeOccurenceBiasingOperation(
    const G4Track *track, const G4BiasingProcessInterface *callingProcess) {
  /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: "
            << "Kinetic energy: " << track->GetKineticEnergy()
            << " MeV" << std::endl;*/

  // if we want to only bias children of primary, leave if this track is NOT a
  // child of the primary
  if (only_children_of_primary_ and track->GetParentID() != 1)
    return 0;

  // is this track too low energy to be biased?
  if (track->GetKineticEnergy() < threshold_)
    return 0;

  /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: "
            << "Calling process: "
            << callingProcess->GetWrappedProcess()->GetProcessName()
            << std::endl;*/

  std::string currentProcess =
      callingProcess->GetWrappedProcess()->GetProcessName();
  if (currentProcess.compare(this->getProcessToBias()) == 0) {
    G4double interactionLength =
        callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();
    /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: "
              << "PN Interaction length: "
              << interactionLength << std::endl;*/

    pnXsecUnbiased_ = 1. / interactionLength;
    /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: Unbiased PN xsec: "
              << pnXsecUnbiased_ << std::endl;*/

    pnXsecBiased_ = pnXsecUnbiased_ * factor_;
    /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: Biased PN xsec: "
              << pnXsecBiased_ << std::endl;*/

    return BiasedXsec(pnXsecBiased_);

  } else if ((currentProcess.compare(CONVERSION_PROCESS) == 0) and
             down_bias_conv_) {
    G4double interactionLength =
        callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();
    /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: "
              << "EM Interaction length: "
              << interactionLength << std::endl;*/

    double emXsecUnbiased = 1. / interactionLength;
    /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: Unbiased EM xsec: "
              << emXsecUnbiased << std::endl;*/

    double emXsecBiased = std::max(
        emXsecUnbiased + pnXsecUnbiased_ - pnXsecBiased_, pnXsecUnbiased_);
    if (emXsecBiased == pnXsecUnbiased_) {
      G4cout << "[ PhotoNuclearXsecBiasingOperator ]: [ WARNING ]: "
             << "Biasing factor is too large." << std::endl;
    }
    /*std::cout << "[ PhotoNuclearXsecBiasingOperator ]: Biased EM xsec: "
              << emXsecBiased << std::endl;*/

    emXsecOperation->SetBiasedCrossSection(emXsecBiased);
    emXsecOperation->Sample();

    return emXsecOperation;

  } else
    return 0;
}
} // namespace g4fire::biasing

DECLARE_XSECBIASINGOPERATOR(g4fire::biasing, PhotoNuclear)