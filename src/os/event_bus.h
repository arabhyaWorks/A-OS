#pragma once

#include <string>
#include <functional>
#include <queue>
#include <map>
#include <vector>

namespace AOS {

/**
 * Event types supported by the OS
 * All input sources (buttons, keyboard, voice, etc.) are normalized to events
 */
enum class EventType {
    // System events
    SYSTEM_STARTUP,
    SYSTEM_SHUTDOWN,

    // Input events (keyboard/gamepad)
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SELECT,     // Enter/A button
    KEY_BACK,       // Escape/B button

    // Voice events (future)
    VOICE_WAKE,
    VOICE_PARTIAL,
    VOICE_FINAL,
    VOICE_COMMAND,

    // App lifecycle events
    APP_STARTED,
    APP_PAUSED,
    APP_RESUMED,
    APP_STOPPED,

    // Custom app events
    CUSTOM
};

/**
 * Event structure
 * Unified event format for all OS communications
 */
struct Event {
    EventType type;
    std::string payload;    // Optional data (e.g., voice text, command params)
    int data_int;          // Optional numeric data

    Event(EventType t, const std::string& p = "", int d = 0)
        : type(t), payload(p), data_int(d) {}
};

/**
 * EventBus - Central pub/sub system for OS-wide communication
 *
 * This is the nervous system of A-OS. All components communicate through events:
 * - Input devices publish events
 * - Apps subscribe to events they care about
 * - System components broadcast state changes
 */
class EventBus {
public:
    using EventHandler = std::function<void(const Event&)>;

    static EventBus& getInstance();

    // Subscribe to specific event type
    void subscribe(EventType type, EventHandler handler);

    // Publish an event (queued for next update cycle)
    void publish(const Event& event);

    // Process queued events (called each frame)
    void processEvents();

    // Clear all subscribers and events
    void clear();

private:
    EventBus() = default;
    ~EventBus() = default;

    // Non-copyable
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    std::queue<Event> eventQueue;
    std::map<EventType, std::vector<EventHandler>> subscribers;
};

} // namespace AOS
