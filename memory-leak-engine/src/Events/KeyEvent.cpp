#include "Events/KeyEvent.h"

namespace mlg {
    Key::KeyCode KeyEvent::GetKeyCode() const {
        return keyCode;
    }

    KeyEvent::KeyEvent(const Key::KeyCode keyCode)
        : keyCode(keyCode) {
    }

    KeyPressedEvent::KeyPressedEvent(Key::KeyCode keyCode, bool isRepeat) : KeyEvent(keyCode), isRepeat(isRepeat) {}

    bool KeyPressedEvent::GetIsRepeat() {
        return isRepeat;
    }

    std::string KeyPressedEvent::toString() {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << keyCode << ", " << isRepeat;
        return ss.str();
    }

    KeyReleasedEvent::KeyReleasedEvent(Key::KeyCode keyCode) : KeyEvent(keyCode) {
    }

    std::string KeyReleasedEvent::toString() {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << keyCode;
        return ss.str();
    }

    KeyTypedEvent::KeyTypedEvent(Key::KeyCode keyCode) : KeyEvent(keyCode) {
    }

    std::string KeyTypedEvent::toString() {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << keyCode;
        return ss.str();
    }
}// namespace mlg