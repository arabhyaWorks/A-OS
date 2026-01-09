#include "event_bus.h"

namespace AOS {

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(EventType type, EventHandler handler) {
    subscribers[type].push_back(handler);
}

void EventBus::publish(const Event& event) {
    eventQueue.push(event);
}

void EventBus::processEvents() {
    // Process all queued events
    while (!eventQueue.empty()) {
        Event event = eventQueue.front();
        eventQueue.pop();

        // Notify all subscribers of this event type
        auto it = subscribers.find(event.type);
        if (it != subscribers.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }
}

void EventBus::clear() {
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
    subscribers.clear();
}

} // namespace AOS
