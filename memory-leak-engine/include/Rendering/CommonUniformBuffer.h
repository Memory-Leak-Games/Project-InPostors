#pragma once

namespace mlg {
    struct CommonUniforms {
        glm::mat4 projection; // 0
        glm::mat4 view;       // 64

        double seconds;        // 128
        float deltaSeconds;   // 136

        uint32_t randInt;     // 140
        float randFloat;      // 144 - 148
        uint64_t fillUpBlock; // 148 - 156
    };

    class CommonUniformBuffer {
    private:
        CommonUniforms uniforms;

        static CommonUniformBuffer* instance;
        uint32_t ubo;

        CommonUniformBuffer() = default;
    public:
        static void Initialize();
        static void Stop();

        static void UpdateAndSendToGPU();

        static const CommonUniforms& GetUniforms();

        friend class Camera;
    };

} // mlg
