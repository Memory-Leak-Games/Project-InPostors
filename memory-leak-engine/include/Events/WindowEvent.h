#pragma once

#include "Event.h"

namespace mlg {

    class WindowResizeEvent : public Event {
    private:
        uint32_t width;
        uint32_t height;

    public:
        WindowResizeEvent(uint32_t width, uint32_t height);

        EVENT_CLASS_TYPE(WindowResize)

        [[nodiscard]] std::string ToString() const override;

        [[nodiscard]] uint32_t GetWidth() const;
        [[nodiscard]] uint32_t GetHeight() const;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose);
    };

    class WindowShouldCloseEvent : public Event {
    public:
        WindowShouldCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose);
    };

    class WindowFocusEvent : public Event {
    private:
        bool isWindowFocused;
    public:
        explicit WindowFocusEvent(bool isWindowFocused);

        [[nodiscard]] bool GetIsWindowFocused() const;
        [[nodiscard]] std::string ToString() const override;

        EVENT_CLASS_TYPE(WindowFocus);
    };

}// namespace mlg
