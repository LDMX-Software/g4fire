#pragma once

#include "fire/Event.h"

class G4Event;

namespace g4fire::event {

/**
 */
class EventBuilder {
 public:
  /**
   */
  EventBuilder() = default;

  /// Destructor
  ~EventBuilder() = default;

  /**
   */
  void writeEvent(const G4Event *g4event, fire::Event &event);

  /**
   */
  void writeHeader(const G4Event *g4event, fire::Event &event);
};
} // namespace g4fire::event
