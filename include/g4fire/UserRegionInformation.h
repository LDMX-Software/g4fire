#pragma once

#include "G4VUserRegionInformation.hh"

namespace g4fire {

/**
 * @brief Defines extra information for a detector region
 *
 * @note
 * This extension to the user region information has a flag indicating
 * whether secondary particles should be stored.  This flag is used
 * in the UserTrackingAction to determine whether or not a trajectory
 * is created for a track created in the region.
 */
class UserRegionInformation : public G4VUserRegionInformation {
 public:
  UserRegionInformation(bool store_secondaries);

  ~UserRegionInformation() = default;

  void Print() const;

  bool getStoreSecondaries() const;

 private:
  bool store_secondaries_;
};

}  // namespace g4fire
