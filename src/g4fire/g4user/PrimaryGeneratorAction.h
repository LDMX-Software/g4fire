#ifndef G4FIRE_G4USER_PRIMARYGENERATORACTION_H
#define G4FIRE_G4USER_PRIMARYGENERATORACTION_H

#include <G4VUserPrimaryGeneratorAction.hh>

#include <fire/config/Parameters.h> 

#include "g4fire/user/PrimaryGenerator.h"

class G4Event;

namespace g4fire::g4user {

/**
 * Implementation of Geant4 primary generator action.
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  /*
   * Constructor
   *
   * @param params The parameters used to configure the primary
   *                   generator action.
   */
  PrimaryGeneratorAction(std::vector<std::unique_ptr<user::PrimaryGenerator>>& pgs);

  /**
   * Class destructor.
   */
  virtual ~PrimaryGeneratorAction() = default;

  /**
   * Generate primaries for the event.
   *
   * This is called by the RunManager before being
   * given to the EventManager to process. This means
   * we must create the UserEventInformation here so
   * that it is accessible for including the weights imported
   * by the primary generators.
   *
   * @see G4RunManager::GenerateEvent for where this method is called
   * @see G4RunManager::ProcessOneEvent for where G4RunManager::GenerateEvent is
   * called
   *
   * Set UserInformation for primary vertices if they haven't been set before.
   *
   * @param event The Geant4 event.
   */
  void GeneratePrimaries(G4Event* event) final override;

 private:
  /// Manager of all generators used by the event
  std::vector<user::PrimaryGenerator*> generators_;
};  // PrimaryGeneratorAction

}  // namespace g4fire::g4user

#endif  // G4FIRE_G4USER_PRIMARYGENERATORACTION_H
