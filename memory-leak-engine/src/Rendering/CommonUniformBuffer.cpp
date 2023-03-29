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


        glGenBuffers(1, &instance->ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, instance->ubo);
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
        instance->uniforms.randFloat = (float) rand() / RAND_MAX;

        glBindBuffer(GL_UNIFORM_BUFFER, instance->ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CommonUniforms), &instance->uniforms);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    const CommonUniforms& CommonUniformBuffer::GetUniforms() {
        return instance->uniforms;
    };
} // mlg