#ifndef SIMCORE_XSECBIASINGOPERATOR_H_
#define SIMCORE_XSECBIASINGOPERATOR_H_

#include "Framework/Configure/Parameters.h"
#include "Framework/RunHeader.h"

//------------//
//   Geant4   //
//------------//
#include "G4BOptnChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4BiasingProcessSharedData.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4VBiasingOperator.hh"

namespace simcore {

/// Forward declaration for generic building function
class XsecBiasingOperator;

/// Define type of building fuction for biasing operators
typedef XsecBiasingOperator* XsecBiasingOperatorBuilder(
    const std::string& name, ldmx::Parameters& parameters);

class XsecBiasingOperator : public G4VBiasingOperator {
 public:
  /** Constructor */
  XsecBiasingOperator(std::string name, const ldmx::Parameters& parameters);

  /** Destructor */
  virtual ~XsecBiasingOperator();

  /**
   * Method used to register an operator with the manager.
   *
   * @param className Name of the class instance
   * @param builder The builder used to create and instance of this class.
   */
  static void declare(const std::string& className,
                      XsecBiasingOperatorBuilder* builder);

  /**
   * @return Method that returns the biasing operation that will be used
   *         to bias the occurence of photonuclear events.
   */
  virtual G4VBiasingOperation* ProposeOccurenceBiasingOperation(
      const G4Track* track,
      const G4BiasingProcessInterface* callingProcess) = 0;

  /** Method called at the beginning of a run. */
  void StartRun();

  /**
   * Return the process whose cross-section will be biased.
   *
   * We need this to be able to check that the process
   * was biased before creating the biasing operator.
   */
  virtual std::string getProcessToBias() const = 0;

  /**
   * Return the particle which should be biased.
   *
   * We need this to be able to tell the physics
   * list which particle to bias.
   * @see RunManager::setupPhysics
   */
  virtual std::string getParticleToBias() const = 0;

  /**
   * Return the volume which should be biased.
   *
   * We need this to be able to tell the detector
   * construction which volumes to attach this
   * operator to.
   */
  virtual std::string getVolumeToBias() const = 0;

  /**
   * Record the configuration of this
   * biasing operator into the run header.
   */
  virtual void RecordConfig(ldmx::RunHeader& header) const = 0;

 protected:
  /**
   * Helper method for passing a biased interaction length
   * to the Geant4 biasing framework.
   *
   * Use like:
   *  return BiasedXsec(biased_xsec);
   * inside of ProposeOccurenceBiasingOperation when
   * you want to update the biased cross section.
   */
  G4VBiasingOperation* BiasedXsec(double biased_xsec) {
    xsecOperation_->SetBiasedCrossSection(biased_xsec);
    xsecOperation_->Sample();
    return xsecOperation_;
  }

  /**
   * Check if the given processed is being biased.
   *
   * @param process Process of interest
   * @return true if the process is being biased, false otherwise
   */
  bool processIsBiased(std::string process);

  /** Cross-section biasing operation. */
  G4BOptnChangeCrossSection* xsecOperation_{nullptr};

  /** Process manager associated with the particle of interest. */
  G4ProcessManager* processManager_{nullptr};

  /**
   * Do *not* propose any biasing on final states.
   */
  G4VBiasingOperation* ProposeFinalStateBiasingOperation(
      const G4Track*, const G4BiasingProcessInterface*) {
    return nullptr;
  }

  /**
   * Do *not* propose any non-physics biasing.
   */
  G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(
      const G4Track*, const G4BiasingProcessInterface*) {
    return nullptr;
  }

};  // XsecBiasingOperator
}  // namespace simcore

/**
 * @macro DECLARE_XSECBIASINGOPERATOR
 *
 * Defines a builder for the declared class
 * and then registers the class as a biasing operator.
 */
#define DECLARE_XSECBIASINGOPERATOR(NS, CLASS)                                 \
  simcore::XsecBiasingOperator* CLASS##Builder(const std::string& name,        \
                                               ldmx::Parameters& parameters) { \
    return new NS::CLASS(name, parameters);                                    \
  }                                                                            \
  __attribute((constructor(405))) static void CLASS##Declare() {               \
    simcore::XsecBiasingOperator::declare(                                     \
        std::string(#NS) + "::" + std::string(#CLASS), &CLASS##Builder);       \
  }

#endif  // SIMCORE_XSECBIASINGOPERATOR_H_
