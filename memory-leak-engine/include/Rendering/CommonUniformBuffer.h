#pragma once

namespace mlg {
    struct CommonUniforms {
        glm::mat4 projection;  // 0
        glm::mat4 view;        // 64

        double seconds;        // 128
        float deltaSeconds;    // 136

        uint32_t randInt;      // 140
        float randFloat;       // 144 - 148
        uint32_t offset;         // 148 - 152
        glm::ivec2 resolution; // 152 - 160


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

        static void SetProjection(const glm::mat4& projection);
        static void SetView(const glm::mat4& view);

        static void UpdateAndSendToGPU();

        static const CommonUniforms& GetUniforms();
    };

} // mlg
