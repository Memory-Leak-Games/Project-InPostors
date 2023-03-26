#include "Rendering/ShaderProgram.h"

#include "glad/glad.h"
#include "glm/ext.hpp"
#include <fstream>

#include "Rendering/Assets/ShaderAsset.h"

#include "Macros.h"

using namespace mlg;

void ShaderProgram::SetFloat(const std::string& name, float value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1f(UniformLocation, value);
}

void ShaderProgram::SetInt(const std::string& name, int value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1i(UniformLocation, value);
}

void ShaderProgram::SetIntArray(const std::string& name, int* values, uint32_t count) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1iv(UniformLocation, count, values);
}

void ShaderProgram::SetBool(const std::string& name, bool value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1i(UniformLocation, static_cast<GLint>(value));
}

void ShaderProgram::SetVec2F(const std::string& name, glm::vec2 value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform2f(UniformLocation, value.x, value.y);
}

void ShaderProgram::SetVec3F(const std::string& name, glm::vec3 value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform3f(UniformLocation, value.x, value.y, value.z);
}

void ShaderProgram::SetVec4F(const std::string& name, glm::vec4 value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform4f(UniformLocation, value.x, value.y, value.z, value.w);
}

void ShaderProgram::SetMat4F(const std::string& name, glm::mat4 value) const {
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}

GLint ShaderProgram::GetUniformLocation(const std::string& name) const {
    GLint UniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());

    return UniformLocation;
}

void ShaderProgram::Activate() const {
    glUseProgram(shaderProgramId);
}

void ShaderProgram::DeActivate() const {
    glUseProgram(0);
}

ShaderProgram::ShaderProgram(const std::shared_ptr<ShaderAsset> &vertexShader,
                             const std::shared_ptr<ShaderAsset> &fragmentShader) :
                             vertexShader(vertexShader), fragmentShader(fragmentShader) {
    LinkProgram(vertexShader->GetShaderID(), fragmentShader->GetShaderID(), 0);
}

ShaderProgram::ShaderProgram(const std::shared_ptr<ShaderAsset> &vertexShader,
                             const std::shared_ptr<ShaderAsset> &fragmentShader,
                             const std::shared_ptr<ShaderAsset> &geometryShader) :
                             vertexShader(vertexShader),
                             fragmentShader(fragmentShader),
                             geometryShader(geometryShader) {
    LinkProgram(vertexShader->GetShaderID(), fragmentShader->GetShaderID(), geometryShader->GetShaderID());
}

void ShaderProgram::LinkProgram(uint32_t vShader, uint32_t fShader, uint32_t gShader = 0) {
    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vShader);
    glAttachShader(shaderProgramId, fShader);

    if (geometryShader != 0) {
        glAttachShader(shaderProgramId, gShader);
    }

    glLinkProgram(shaderProgramId);

    GLint ProgramLinkingResult;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &ProgramLinkingResult);
    if (!ProgramLinkingResult) {
        char Log[512];
        glGetProgramInfoLog(shaderProgramId, 512, nullptr, Log);
        SPDLOG_ERROR("Program linking failed: " + std::string(Log));
        MLG_ASSERT(ProgramLinkingResult);
    }
}

GLint ShaderProgram::TrySetVec4f(const std::string& name, glm::vec4 value) const {
    GLint UniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    return UniformLocation;
}

