#pragma once

#include <string>

#include "fire/Processor.h"
#include "fire/exception/Exception.h"

namespace g4fire {

/**
 * Handle to the conditions system, provided at construction to classes
 * which require it.
 */
class ConditionsInterface {
 public:
  ConditionsInterface(fire::Processor *p) : processor_{p} {}

  /**
   * Primary request action for a conditions object If the
   * object is in the cache and still valid (IOV), the
   * cached object will be returned.  If it is not in the cache,
   * or is out of date, the () method will be called to provide the
   * object.
   */
  template <class T> const T &getCondition(const std::string &condition_name) {
    if (processor_ == nullptr) {
      throw fire::Exception(
          "Conditions", "Failed to find a conditions object in g4fire", false);
    }
    return processor_->getCondition<T>(condition_name);
  }

 private:
  /// Pointer to the owner processor object
  fire::Processor *processor_;

}; // ConditionsInterface
} // namespace g4fire
