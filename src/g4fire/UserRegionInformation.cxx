#include "g4fire/UserRegionInformation.h"

namespace g4fire {

UserRegionInformation::UserRegionInformation(bool store_secondaries)
    : store_secondaries_(store_secondaries) {}

UserRegionInformation::~UserRegionInformation() {}

bool UserRegionInformation::getStoreSecondaries() const {
  return store_secondaries_;
}

void UserRegionInformation::Print() const {}

}  // namespace g4fire
