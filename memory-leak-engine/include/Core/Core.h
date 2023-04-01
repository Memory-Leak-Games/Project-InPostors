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
    };
}// namespace mlg
