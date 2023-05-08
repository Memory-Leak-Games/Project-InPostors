#pragma once

namespace mlg {

    class FrameBuffer {
    private:
        uint32_t fbo = 0;

        int32_t width;
        int32_t height;

    protected:
        std::shared_ptr<class MaterialAsset> material;

    public:
        FrameBuffer() = delete;

        FrameBuffer(int32_t width, int32_t height);
        virtual ~FrameBuffer() = 0;

        bool IsFrameBufferComplete();

        virtual void Clear();
        virtual void Activate();
        virtual void Draw();

        virtual void Resize(int32_t width, int32_t height);

        void CopyDepthBuffer(uint32_t fbo = 0);

        uint32_t GetFbo();
    protected:
        int32_t GetWidth();
        int32_t GetHeight();
    };

} // mlg
