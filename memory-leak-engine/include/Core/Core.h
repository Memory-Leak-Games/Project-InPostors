#pragma once

#include <cstdint>
#include <eventpp/callbacklist.h>

#include "Window.h"

namespace mlg {
    class Core {
    private:
        static Core* instance;

        Core() = default;

    public:
        eventpp::CallbackList<void()> OnUpdate;

        static void Initialize();
        static void Stop();

        static Core* GetInstance();

        void MainLoop();

        void RenderImGUI() const;

        void TickRendering() const;
        void TickGameplay() const;
        void TickWindow() const;
    };
}// namespace mlg
