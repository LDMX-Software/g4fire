#include "g4fire/XsecBiasingOperator.h"

//#include "Framework/Exception/Exception.h"
#include "g4fire/PluginFactory.h"

namespace g4fire {

XsecBiasingOperator::XsecBiasingOperator(
    std::string name, const fire::config::Parameters& params)
    : G4VBiasingOperator(name) {}

XsecBiasingOperator::~XsecBiasingOperator() {}

void XsecBiasingOperator::StartRun() {
  if (this->getParticleToBias().compare("gamma") == 0) {
    process_manager_ = G4Gamma::GammaDefinition()->GetProcessManager();
  } else if (this->getParticleToBias().compare("e-") == 0) {
    process_manager_ = G4Electron::ElectronDefinition()->GetProcessManager();
  } else if (this->getParticleToBias().compare("neutron") == 0) {
    process_manager_ = G4Neutron::NeutronDefinition()->GetProcessManager();
  } else if (this->getParticleToBias().compare("kaon0L") == 0) {
    process_manager_ = G4KaonZeroLong::KaonZeroLongDefinition()->GetProcessManager();
  } else {
    //EXCEPTION_RAISE("BiasSetup", "Invalid particle type '" +
    //                                 this->getParticleToBias() + "'.");
  }

  // TODO(OM): Use logger instead
  std::cout << "[ XsecBiasingOperator ]: Biasing particles of type "
            << this->getParticleToBias() << std::endl;

  if (processIsBiased(this->getProcessToBias())) {
    xsec_operation_ =
        new G4BOptnChangeCrossSection("changeXsec-" + this->getProcessToBias());
  } else {
    //EXCEPTION_RAISE("BiasSetup",
    //                this->getProcessToBias() +
    //                    " is not found in list of biased processes!");
  }
}

bool XsecBiasingOperator::processIsBiased(std::string process) {
  // Loop over all processes and check if the given process is being
  // biased.
  const G4BiasingProcessSharedData* shared_data{
      G4BiasingProcessInterface::GetSharedData(process_manager_)};
  if (shared_data) {
    for (size_t iprocess = 0;
         iprocess < (shared_data->GetPhysicsBiasingProcessInterfaces()).size();
         ++iprocess) {
      const G4BiasingProcessInterface* wrapperProcess =
          (shared_data->GetPhysicsBiasingProcessInterfaces())[iprocess];

      if (wrapperProcess->GetWrappedProcess()->GetProcessName().compareTo(
              process) == 0) {
        return true;
      }
    }
  }
  return false;
}

void XsecBiasingOperator::declare(const std::string& class_name,
                                  XsecBiasingOperatorBuilder* builder) {
  PluginFactory::getInstance().registerBiasingOperator(class_name, builder);
}

}  // namespace g4fire
