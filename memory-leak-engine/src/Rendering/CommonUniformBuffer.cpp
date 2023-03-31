#include "Rendering/CommonUniformBuffer.h"

#include "Core/Time.h"
#include "Macros.h"

#include "glad/glad.h"

namespace mlg {
    CommonUniformBuffer* CommonUniformBuffer::instance;
    void CommonUniformBuffer::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing CommonUniformBuffer");
        instance = new CommonUniformBuffer();


        glCreateBuffers(1, &instance->ubo);
        glNamedBufferData(instance->ubo, sizeof(CommonUniforms), nullptr, GL_DYNAMIC_DRAW);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(CommonUniforms), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, instance->ubo);
    }

    void CommonUniformBuffer::Stop() {
        if (instance == nullptr)
            return;

        SPDLOG_INFO("Stopping CommonUniformBuffer");

        glDeleteBuffers(1, &instance->ubo);

        delete instance;
        instance = 0;
    }

    void CommonUniformBuffer::UpdateAndSendToGPU() {
        instance->uniforms.seconds = Time::GetSeconds();
        instance->uniforms.deltaSeconds = Time::GetDeltaSeconds();

        instance->uniforms.randInt = rand();
        instance->uniforms.randFloat = (float) rand() / (float) RAND_MAX;

        glNamedBufferSubData(instance->ubo, 0, sizeof(CommonUniforms), &instance->uniforms);
    }

    const CommonUniforms& CommonUniformBuffer::GetUniforms() {
        return instance->uniforms;
    };
} // mlg