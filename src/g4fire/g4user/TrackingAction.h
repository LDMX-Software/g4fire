#ifndef G4FIRE_G4USER_TRACKINGACTION_H_
#define G4FIRE_G4USER_TRACKINGACTION_H_

#include <vector>

#include <G4RunManager.hh>
#include <G4UserTrackingAction.hh>

#include "g4fire/user/Action.h"

namespace g4fire::g4user {

/**
 * @brief Implementation of user tracking action
 *
 * Here, we manage the interaction between our track storage machinery
 * (TrackMap) and Geant4's tracking manager (G4TrackingManager).
 */
class TrackingAction : public G4UserTrackingAction {
 public:
  /// Class constructor.
  TrackingAction() = default;

  /// Class destructor.
  virtual ~TrackingAction() = default;

  /**
   * Implementation of pre-tracking action.
   *
   * This is called whenever a track is going to start
   * being processed.
   *
   * @note A track could go through this function more than
   * once in an event if it is suspended.
   *
   * We call any other tracking actions'
   * 'PreUserTrackingAction' methods.
   *
   * @param track The Geant4 track.
   */
  void PreUserTrackingAction(const G4Track* track);

  /**
   * Implementation of post-tracking action.
   *
   * We start by calling any other tracking actions'
   * PostUserTrackingAction methods.
   *
   * @param track The Geant4 track.
   */
  void PostUserTrackingAction(const G4Track* track);

  /**
   * Register a user action of type RunAction with this class.
   *
   * @param action  User action of type RunAction
   */
  void attach(user::Action* tracking_action) {
    tracking_actions_.push_back(tracking_action);
  }

 private:
  /// custom user actions to be called before and after processing a track
  std::vector<user::Action*> tracking_actions_;
};  // TrackingAction
}  // namespace g4fire::g4user

#endif
