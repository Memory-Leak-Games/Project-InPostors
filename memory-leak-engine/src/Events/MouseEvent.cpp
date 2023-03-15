#include "include/Events/MouseEvent.h"

std::string mlg::MouseMovedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << mouseX << ", " << mouseY;
    return ss.str();
}

float mlg::MouseMovedEvent::GetX() const { return mouseX; }

float mlg::MouseMovedEvent::GetY() const { return mouseY; }

mlg::MouseMovedEvent::MouseMovedEvent(float x, float y)
    : mouseX(x), mouseY(y) {}

mlg::MouseScrolledEvent::MouseScrolledEvent(float xOffset, float yOffset)
    : xOffset(xOffset), yOffset(yOffset) {}

float mlg::MouseScrolledEvent::GetXOffset() const { return xOffset; }

float mlg::MouseScrolledEvent::GetYOffset() const { return yOffset; }

std::string mlg::MouseScrolledEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
    return ss.str();
}

mlg::Mouse::MouseCode mlg::MouseButtonEvent::GetMouseButton() const { return button; }

mlg::MouseButtonEvent::MouseButtonEvent(mlg::Mouse::MouseCode button)
    : button(button) {}

std::string mlg::MouseButtonPressedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << button;
    return ss.str();
}

mlg::MouseButtonPressedEvent::MouseButtonPressedEvent(mlg::Mouse::MouseCode button)
    : MouseButtonEvent(button) {}

mlg::MouseButtonReleasedEvent::MouseButtonReleasedEvent(const mlg::Mouse::MouseCode button)
    : MouseButtonEvent(button) {}

std::string mlg::MouseButtonReleasedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << button;
    return ss.str();
}
