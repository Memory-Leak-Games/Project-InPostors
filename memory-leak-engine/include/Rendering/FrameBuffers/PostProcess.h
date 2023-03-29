#pragma once

#include "Rendering/ShaderProgram.h"

namespace mlg {

    class PostProcess {
    private:
        uint32_t frameBuffer = 0;
        uint32_t colorTexture = 0;
        uint32_t rboDepth = 0;

        std::shared_ptr<class MaterialAsset> material;

        int32_t width;
        int32_t height;

        PostProcess() = default;
    public:
        PostProcess(int32_t width, int32_t height);
        ~PostProcess();

        virtual void Activate();
        virtual void DeActivate();

        virtual void Draw();

        virtual void Resize(int32_t width, int32_t height);
        void CopyDepthBuffer(uint32_t fbo = 0);

        uint32_t GetFrameBuffer() const;

    private:
        void InitializeFbo();

        void GenerateAndBindTextures() ;

    };

} // mlg
