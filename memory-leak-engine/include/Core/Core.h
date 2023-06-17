#pragma once

#include <cstdint>
#include <eventpp/callbacklist.h>

#include "Window.h"

namespace mlg {
    class Core {
    private:
        static Core* instance;

        Core() = default;

        bool shouldClose = false;
        bool closed = true;

    public:
        eventpp::CallbackList<void()> OnUpdate;
        eventpp::CallbackList<void()> OnMainLoopEnd;

        static void Initialize();
        static void Stop();

        static Core* Get();

        void MainLoop();

        void RenderImGUI() const;

        void TickRendering() const;
        void TickGameplay() const;
        void TickWindow() const;

        void Close();
        bool IsClosed() const;
    };
}// namespace mlg
