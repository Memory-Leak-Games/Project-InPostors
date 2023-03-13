#pragma once

#include "Macros.h"

namespace mlg {

    enum class EventType {
        None = 0,

        // Window
        WindowClose,
        WindowResize,
        WindowFocus,

        // App
        AppTick,
        AppUpdate,
        AppRender,

        // Key
        KeyPressed,
        KeyReleased,
        KeyTyped,

        // Mouse
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    class Event {
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType GetEventType() const = 0;
        [[nodiscard]] virtual const char* GetName() const = 0;
        [[nodiscard]] virtual std::string ToString() const { return GetName(); }
    };

    struct EventPolicies {
        static EventType getEvent(const Event& e);
    };

#define EVENT_CLASS_TYPE(type)                                                  \
    static EventType GetStaticType() { return EventType::type; }                \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

}
