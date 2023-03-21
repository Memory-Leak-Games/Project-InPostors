#include "Events/WindowEvent.h"

namespace mlg {
    WindowResizeEvent::WindowResizeEvent(int32_t width, int32_t height) : width(width), height(height) {}

    int32_t WindowResizeEvent::GetWidth() const {
        return width;
    }

    int32_t WindowResizeEvent::GetHeight() const {
        return height;
    }

    std::string WindowResizeEvent::ToString() const {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << width << ", " << height;
        return ss.str();
    }

    WindowFocusEvent::WindowFocusEvent(bool isWindowFocused) : isWindowFocused(isWindowFocused) {}

    bool WindowFocusEvent::GetIsWindowFocused() const {
        return isWindowFocused;
    }

    std::string WindowFocusEvent::ToString() const {
        std::stringstream ss;
        ss << "WindowFocusEvent, is window focused: "<< isWindowFocused;
        return ss.str();
    }
}// namespace mlg