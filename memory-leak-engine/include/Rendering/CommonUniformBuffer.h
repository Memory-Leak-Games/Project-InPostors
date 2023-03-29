#pragma once

namespace mlg {
    struct CommonUniforms {
        glm::mat4 projection; // 0 - 64
        glm::mat4 view;       // 64 - 128

        float seconds;        // 128 - 132
        float deltaSeconds;   // 136 - 140

        uint32_t randInt;     // 144 - 148
        float randFloat;      // 148 - 152
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
