#pragma once

#include "Core/HID/KeyCodes.h"
#include "Event.h"

namespace mlg {
    class MouseMovedEvent : public Event {
    private:
        float mouseX;
        float mouseY;

    public:
        MouseMovedEvent(float x, float y);

        [[nodiscard]] float GetX() const;
        [[nodiscard]] float GetY() const;

        [[nodiscard]] std::string ToString() const override;

        EVENT_CLASS_TYPE(MouseMoved)
    };

    class MouseScrolledEvent : public Event {
    private:
        float xOffset, yOffset;

    public:
        MouseScrolledEvent(float xOffset, float yOffset);

        [[nodiscard]] float GetXOffset() const;
        [[nodiscard]] float GetYOffset() const;

        [[nodiscard]] std::string ToString() const override;

        EVENT_CLASS_TYPE(MouseScrolled)
    };

    class MouseButtonEvent : public Event {
    protected:
        Mouse::MouseCode button;

    public:
        [[nodiscard]] Mouse::MouseCode GetMouseButton() const;

    protected:
        explicit MouseButtonEvent(Mouse::MouseCode button);
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        explicit MouseButtonPressedEvent(Mouse::MouseCode button);

        [[nodiscard]] std::string ToString() const override;

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        explicit MouseButtonReleasedEvent(const Mouse::MouseCode button);

        [[nodiscard]] std::string ToString() const override;

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}// namespace mlg
