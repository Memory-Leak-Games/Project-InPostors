#pragma once

namespace mlg {

    class UIRenderable {
    public:
        virtual void Draw(const class UIRenderer* renderer) = 0;
    };

}