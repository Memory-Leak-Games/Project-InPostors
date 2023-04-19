#include "Rendering/CommonUniformBuffer.h"

#include "Core/Time.h"
#include "Macros.h"

#include "Core/Window.h"

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
        ZoneScoped;

        using Random = effolkronium::random_static;

        instance->uniforms.seconds = Time::GetSeconds();
        instance->uniforms.deltaSeconds = Time::GetDeltaSeconds();

        instance->uniforms.randInt = Random::get<uint32_t>();
        instance->uniforms.randFloat = Random::get(0.f, 1.f);

        Window* window = Window::GetInstance();
        instance->uniforms.resolution = {window->GetWidth(), window->GetHeight()};

        glNamedBufferSubData(instance->ubo, 0, sizeof(CommonUniforms), &instance->uniforms);
    }

    const CommonUniforms& CommonUniformBuffer::GetUniforms() {
        return instance->uniforms;
    }

    void CommonUniformBuffer::SetProjection(const glm::mat4& projection) {
        instance->uniforms.projection = projection;
    }

    void CommonUniformBuffer::SetView(const glm::mat4& view) {
        instance->uniforms.view = view;
    };
} // mlg