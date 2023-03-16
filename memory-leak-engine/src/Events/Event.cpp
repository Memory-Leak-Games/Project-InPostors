#include "Events/Event.h"

namespace mlg {
    EventType EventPolicies::getEvent(const Event& e) {
        return e.GetEventType();
    }
}