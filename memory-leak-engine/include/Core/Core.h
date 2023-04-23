#pragma once

#include <cstdint>

#include "Window.h"

namespace mlg {
    class Core {
    private:
        static Core* instance;

        Core() = default;

    public:
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
