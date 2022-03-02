#include "g4fire/UserRegionInformation.h"

namespace g4fire {

UserRegionInformation::UserRegionInformation(bool aStoreSecondaries)
    : storeSecondaries_(aStoreSecondaries) {}

UserRegionInformation::~UserRegionInformation() {}

bool UserRegionInformation::getStoreSecondaries() const {
  return storeSecondaries_;
}

void UserRegionInformation::Print() const {}

}  // namespace g4fire
