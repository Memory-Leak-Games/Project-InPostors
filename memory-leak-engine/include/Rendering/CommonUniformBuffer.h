#pragma once

namespace mlg {
    struct CommonUniforms {
        glm::mat4 projection; // 0 - 16
        glm::mat4 view;       // 16 - 32

        float seconds;        // 32 - 36
        float deltaSeconds;   // 36 - 40

        uint32_t randInt;     // 40 - 44
        float randFloat;      // 44 - 48
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
