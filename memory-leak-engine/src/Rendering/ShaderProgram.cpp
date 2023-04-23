#include "Rendering/ShaderProgram.h"

#include "glad/glad.h"
#include "glm/ext.hpp"
#include <fstream>

#include "Rendering/Assets/ShaderAsset.h"

#include "Macros.h"

using namespace mlg;

void ShaderProgram::SetFloat(const std::string& name, float value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1f(uniformLocation, value);
}

void ShaderProgram::SetInt(const std::string& name, int value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1i(uniformLocation, value);
}

void ShaderProgram::SetIntArray(const std::string& name, int* values, uint32_t count) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1iv(uniformLocation, count, values);
}

void ShaderProgram::SetBool(const std::string& name, bool value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1i(uniformLocation, static_cast<GLint>(value));
}

void ShaderProgram::SetVec2F(const std::string& name, glm::vec2 value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform2f(uniformLocation, value.x, value.y);
}

void ShaderProgram::SetVec3F(const std::string& name, glm::vec3 value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform3f(uniformLocation, value.x, value.y, value.z);
}

void ShaderProgram::SetVec4F(const std::string& name, glm::vec4 value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
}

void ShaderProgram::SetMat4F(const std::string& name, glm::mat4 value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::SetMat3F(const std::string& name, glm::mat3 value) const {
    GLint uniformLocation = GetUniformLocation(name);
    if (uniformLocation == -1) {
        SPDLOG_WARN(name + " not found");
    }
    glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}

int32_t ShaderProgram::GetUniformLocation(const std::string& name) const {
    if (uniformCache.find(name) != uniformCache.end())
        return uniformCache[name];

    int32_t uniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());
    uniformCache[name] = uniformLocation;

    return uniformLocation;
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

    int32_t programLinkingResult;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &programLinkingResult);
    if (!programLinkingResult) {
        char _log[512];
        glGetProgramInfoLog(shaderProgramId, 512, nullptr, _log);
        SPDLOG_ERROR("Program linking failed: " + std::string(_log));
        MLG_ASSERT(programLinkingResult);
    }
}

int32_t ShaderProgram::TrySetVec4f(const std::string& name, glm::vec4 value) const {
    int32_t uniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    return uniformLocation;
}


