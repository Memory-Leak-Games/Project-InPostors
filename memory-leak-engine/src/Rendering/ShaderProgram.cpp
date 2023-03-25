#include "Rendering/ShaderProgram.h"

#include "glad/glad.h"
#include "glm/ext.hpp"
#include <fstream>

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

std::string ShaderProgram::LoadShader(const std::string& shaderPath) {
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    shaderFile.open(shaderPath);
    std::stringstream ShaderStream;

    ShaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return ShaderStream.str();
}

ShaderProgram::ShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath) : ShaderProgram(
        std::move(vertexShaderPath), std::move(fragmentShaderPath), "") {

}

ShaderProgram::ShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath,
                             std::string geometryShaderPath) {
    GLuint VertexShader, FragmentShader;

    VertexShader = CompileVertexShader(vertexShaderPath);
    FragmentShader = CompileFragmentShader(fragmentShaderPath);
    if (!geometryShaderPath.empty()) {
        GLuint GeometryShader = CompileGeometryShader(geometryShaderPath);
        LinkProgram(VertexShader, FragmentShader, GeometryShader);
    } else {
        LinkProgram(VertexShader, FragmentShader, 0);
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
}


void ShaderProgram::LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0) {
    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShader);
    glAttachShader(shaderProgramId, fragmentShader);

    if (geometryShader != 0) {
        glAttachShader(shaderProgramId, geometryShader);
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

GLuint ShaderProgram::CompileFragmentShader(const std::string& fragmentShaderPath) {
    GLuint FragmentShader;
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(fragmentShaderPath, FragmentShader);
    LogShaderError(FragmentShader, "Fragment Shader compilation failed: ");

    return FragmentShader;
}

GLuint ShaderProgram::CompileVertexShader(const std::string& vertexShaderPath) {
    GLuint VertexShader;
    VertexShader = glCreateShader(GL_VERTEX_SHADER);

    CompileShader(vertexShaderPath, VertexShader);
    LogShaderError(VertexShader, "Vertex Shader compilation failed: ");

    return VertexShader;
}

GLuint ShaderProgram::CompileGeometryShader(const std::string& geometryShaderPath) {
    GLuint GeometryShader;
    GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    CompileShader(geometryShaderPath, GeometryShader);
    LogShaderError(GeometryShader, "Geometry Shader compilation failed: ");

    return GeometryShader;
}

void ShaderProgram::LogShaderError(GLuint geometryShader, const std::string& message) {
    GLint ShaderCompilationResult;
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &ShaderCompilationResult);
    if (!ShaderCompilationResult) {
        char Log[512] = "Error";
        glGetShaderInfoLog(geometryShader, 512, nullptr, Log);
        SPDLOG_ERROR(message + std::string(Log));
    }
}

void ShaderProgram::CompileShader(const std::string& shaderPath, GLuint shader) {
    std::string ShaderCode = LoadShader(shaderPath);
    const GLchar* ConstCharPtrShaderCode = ShaderCode.c_str();
    glShaderSource(shader, 1, &ConstCharPtrShaderCode, nullptr);
    glCompileShader(shader);
}

GLint ShaderProgram::TrySetVec4f(const std::string& name, glm::vec4 value) const {
    GLint UniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    return UniformLocation;
}

