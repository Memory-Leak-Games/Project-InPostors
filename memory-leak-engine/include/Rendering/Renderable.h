#pragma once

namespace mlg {

    class Renderable {
    public:
        virtual void Draw(class Renderer* renderer) = 0;
    };

} // mlg
