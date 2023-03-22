#pragma once

namespace mlg {

    class Renderable {
    public:
        virtual void Draw(class Renderer* renderer) = 0;
        virtual void LateDraw(Renderer* renderer) = 0;
        virtual ~Renderable() = 0;
    };

} // mlg
