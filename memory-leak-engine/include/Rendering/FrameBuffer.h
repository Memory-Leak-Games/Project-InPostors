#pragma once

#include "ShaderProgram.h"
#include "Rendering/Assets/MaterialAsset.h"

namespace mlg {

    class FrameBuffer {
    private:
        uint32_t frameBuffer;
        uint32_t colorTexture;
        uint32_t depthStencilTexture;

        uint32_t vao;
        uint32_t vbo;

        std::shared_ptr<MaterialAsset> material;

        FrameBuffer() = default;
    public:
        FrameBuffer(int32_t resolutionX, int32_t resolutionY);
        ~FrameBuffer();

        virtual void Activate();
        virtual void DeActivate();

        virtual void Draw();
        virtual void Clear(glm::vec4 color);

        virtual void Resize(int32_t x, int32_t y);

    private:
        void InitializeVao();
        void InitializeFbo(int32_t resolutionX, int32_t resolutionY);
    };

} // mlg
