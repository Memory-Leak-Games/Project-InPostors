#pragma once

#include "Event.h"
#include "Core/HID/KeyCodes.h"

namespace mlg {

    class KeyEvent : public Event{
    protected:
        Key::KeyCode keyCode;

    public:
        [[nodiscard]] Key::KeyCode GetKeyCode() const;

    protected:
        explicit KeyEvent(Key::KeyCode keyCode);
    };

    class KeyPressedEvent : public KeyEvent {
    private:
        bool isRepeat;

    public:
        explicit KeyPressedEvent(Key::KeyCode keyCode, bool isRepeat = false);

        bool GetIsRepeat();

        std::string toString();

        EVENT_CLASS_TYPE(KeyPressed)
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        explicit KeyReleasedEvent(Key::KeyCode keyCode);

        std::string toString();

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        explicit KeyTypedEvent(Key::KeyCode keyCode);

        std::string toString();

        EVENT_CLASS_TYPE(KeyReleased)
    };
}// namespace mlg
