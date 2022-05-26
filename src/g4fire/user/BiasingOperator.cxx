#include "g4fire/user/BiasingOperator.h"

#include <G4BiasingProcessSharedData.hh>
#include <G4Electron.hh>
#include <G4Gamma.hh>
#include <G4Neutron.hh>
#include <G4KaonZeroLong.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4ProcessManager.hh>
#include <G4RunManager.hh>

#include <fire/exception/Exception.h>

namespace g4fire::user {

BiasingOperator::BiasingOperator(const fire::config::Parameters& p)
  : G4VBiasingOperator(p.get<std::string>("name")) {}

void BiasingOperator::StartRun() {
  if (this->getParticleToBias().compare("gamma") == 0) {
    process_manager_ = G4Gamma::GammaDefinition()->GetProcessManager();
  } else if (this->getParticleToBias().compare("e-") == 0) {
    process_manager_ = G4Electron::ElectronDefinition()->GetProcessManager();
  } else if (this->getParticleToBias().compare("neutron") == 0) {
    process_manager_ = G4Neutron::NeutronDefinition()->GetProcessManager();
  } else if (this->getParticleToBias().compare("kaon0L") == 0) {
    process_manager_ = G4KaonZeroLong::KaonZeroLongDefinition()->GetProcessManager();
  } else {
    throw fire::Exception("BiasSetup",
        "Invalid particle type '"+this->getParticleToBias()
        +"'.");
  }

  if (processIsBiased(this->getProcessToBias())) {
    xsec_operation_ =
        new G4BOptnChangeCrossSection("changeXsec-" + this->getProcessToBias());
  } else {
    throw fire::Exception("BiasSetup",
        this->getProcessToBias() + " is not found in list of biased processes.");
  }
}

G4VBiasingOperation* BiasingOperator::BiasedXsec(double biased_xsec) {
  xsec_operation_->SetBiasedCrossSection(biased_xsec);
  xsec_operation_->Sample();
  return xsec_operation_;
}

bool BiasingOperator::processIsBiased(const std::string& process) {
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


}
