#pragma once

#include "ShaderProgram.h"

namespace mlg {

    class PostProcess {
    private:
        uint32_t frameBuffer;
        uint32_t colorTexture;
        uint32_t rboDepth;

        std::shared_ptr<class MaterialAsset> material;

        PostProcess() = default;
    public:
        PostProcess(int32_t resolutionX, int32_t resolutionY);
        ~PostProcess();

        virtual void Activate();
        virtual void DeActivate();

        virtual void Draw();
        virtual void Clear(glm::vec4 color);

        virtual void Resize(int32_t x, int32_t y);

        uint32_t GetFrameBuffer() const;

    private:
        void InitializeFbo(int32_t resolutionX, int32_t resolutionY);

        void GenerateAndBindTextures(int32_t resolutionX, int32_t resolutionY) ;
    };

} // mlg
