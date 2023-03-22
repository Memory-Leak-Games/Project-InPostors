#pragma once

#include "Renderable.h"

namespace mlg {

    class Renderer {
    private:
        static Renderer* instance;

        //TODO: Add camera
    public:
        static void Initialize();
        static void Stop();

        static void Draw(Renderable* renderable);
        static void LateDraw(Renderable* renderable);
    };

} // mlg
