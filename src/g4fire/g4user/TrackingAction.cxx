#include "TrackingAction.h"

#include <G4PrimaryParticle.hh>
#include <G4VUserPrimaryParticleInformation.hh>

#include <iostream>

#include "g4fire/TrackMap.h"
#include "g4fire/UserPrimaryParticleInformation.h"
#include "g4fire/UserRegionInformation.h"
#include "g4fire/UserTrackInformation.h"

namespace g4fire::g4user {

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
  if (!track_map_.contains(track)) {
    // New Track
    
    // get track information and initialize our new track
    //  this will create a new track info object if it doesn't exist
    auto track_info{UserTrackInformation::get(track)};
    track_info->initialize(track);

    // Get the region info for where the track was created (could be NULL)
    auto region_info = (UserRegionInformation*)track->GetLogicalVolumeAtVertex()
                          ->GetRegion()
                          ->GetUserInformation();

    // Get the gen status if track was primary
    int cur_gen_status = -1;
    if (track->GetDynamicParticle()->GetPrimaryParticle()) {
      auto primaryInfo = dynamic_cast<UserPrimaryParticleInformation*>(
          track->GetDynamicParticle()
              ->GetPrimaryParticle()
              ->GetUserInformation());
      cur_gen_status = primaryInfo->getHepEvtStatus();
    }

    /**
     * Always save a particle if any of the following are true
     *    it has gen status == 1 (primary)
     *    it is in a region without region info
     *    it is in a region that is marked to store secondaries
     * DON'T change the save-status even if these are false
     *  The track's save-status is false by default when the track-info
     *  is constructed and the track's save-status could have been modified by a
     *  user action **prior** to the track being processed for the first time. 
     *  For example, this happens if the user wants to save the
     *  secondaries of a particular track.
     */
    if (cur_gen_status == 1 or !region_info or region_info->getStoreSecondaries()) {
      track_info->setSaveFlag(true); 
    }

    // insert this track into the event's track map
    track_map_.insert(track);
  }

  // Activate user tracking actions
  for (auto& tracking_action : tracking_actions_)
    tracking_action->PreUserTrackingAction(track);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {
  // Activate user tracking actions
  for (auto& tracking_action : tracking_actions_)
    tracking_action->PostUserTrackingAction(track);

  /**
   * If a track is to-be saved and it is being killed,
   * save the track into the map. This is where a track
   * is chosen to be put into the output particle map.
   * If its save flag is true **for any reason** at this
   * point, then it will be in the output map.
   */
  auto track_info{UserTrackInformation::get(track)};
  if (track_info->getSaveFlag() and
      track->GetTrackStatus() == G4TrackStatus::fStopAndKill) {
    track_map_.save(track);
  }
}

}  // namespace g4fire
