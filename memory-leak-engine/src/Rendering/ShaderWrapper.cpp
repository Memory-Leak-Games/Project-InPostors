#include "Rendering/ShaderWrapper.h"

#include "glm/ext.hpp"
#include <fstream>

#include "Macros.h"

using namespace mlg;

void ShaderWrapper::SetFloat(const std::string& name, float value) const
{
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1)
    {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1f(UniformLocation, value);
}

void ShaderWrapper::SetInt(const std::string& name, int value) const
{
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1)
    {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1i(UniformLocation, value);
}

void ShaderWrapper::SetBool(const std::string& name, bool value) const
{
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1)
    {
        SPDLOG_WARN(name + " not found");
    }
    glUniform1i(UniformLocation, static_cast<GLint>(value));
}

void ShaderWrapper::SetVec4F(const std::string& name, glm::vec4 value) const
{
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1)
    {
        SPDLOG_WARN(name + " not found");
    }
    glUniform4f(UniformLocation, value.x, value.y, value.z, value.w);
}

void ShaderWrapper::SetMat4F(const std::string& name, glm::mat4 value) const
{
    GLint UniformLocation = GetUniformLocation(name);
    if (UniformLocation == -1)
    {
        SPDLOG_WARN(name + " not found");
    }
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}

GLint ShaderWrapper::GetUniformLocation(const std::string& name) const
{
    GLint UniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());

    return UniformLocation;
}

GLuint ShaderWrapper::GetShaderProgramId() const
{
    return shaderProgramId;
}

void ShaderWrapper::Activate() const
{
    glUseProgram(shaderProgramId);
}

void ShaderWrapper::DeActivate() const {
    glUseProgram(0);
}

void ShaderWrapper::LoadShader(std::string& shaderPath, std::string& shaderCodeOut)
{
    std::ifstream ShaderFile;

    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        ShaderFile.open(shaderPath);
        std::stringstream ShaderStream;

        ShaderStream << ShaderFile.rdbuf();
        ShaderFile.close();

        shaderCodeOut = ShaderStream.str();
    } catch (std::ifstream::failure& e)
    {
        SPDLOG_ERROR("Shader file loading failure");
    }
}

ShaderWrapper::ShaderWrapper(std::string vertexShaderPath, std::string fragmentShaderPath) : ShaderWrapper(
        std::move(vertexShaderPath), std::move(fragmentShaderPath), "${-1}")
{

}

ShaderWrapper::ShaderWrapper(std::string vertexShaderPath, std::string fragmentShaderPath,
                             std::string geometryShaderPath)
{
    GLuint VertexShader, FragmentShader;

    VertexShader = CompileVertexShader(vertexShaderPath);
    FragmentShader = CompileFragmentShader(fragmentShaderPath);
    if (geometryShaderPath != "${-1}")
    {
        GLuint GeometryShader = CompileGeometryShader(geometryShaderPath);
        LinkProgram(VertexShader, FragmentShader, GeometryShader);
    }
    else
    {
        LinkProgram(VertexShader, FragmentShader, 0);
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

}


void ShaderWrapper::LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0)
{
    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShader);
    glAttachShader(shaderProgramId, fragmentShader);

    if (geometryShader != 0)
    {
        glAttachShader(shaderProgramId, geometryShader);
    }

    glLinkProgram(shaderProgramId);

    GLint ProgramLinkingResult;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &ProgramLinkingResult);
    if (!ProgramLinkingResult)
    {
        char Log[512];
        glGetProgramInfoLog(shaderProgramId, 512, nullptr, Log);
        SPDLOG_ERROR("Program linking failed: " + std::string(Log));
    }
}

GLuint ShaderWrapper::CompileFragmentShader(std::string& fragmentShaderPath)
{
    GLuint FragmentShader;
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(fragmentShaderPath, FragmentShader);
    LogShaderError(FragmentShader, "Fragment Shader compilation failed: ");

    return FragmentShader;
}

GLuint ShaderWrapper::CompileVertexShader(std::string& vertexShaderPath)
{
    GLuint VertexShader;
    VertexShader = glCreateShader(GL_VERTEX_SHADER);

    CompileShader(vertexShaderPath, VertexShader);
    LogShaderError(VertexShader, "Vertex Shader compilation failed: ");

    return VertexShader;
}

GLuint ShaderWrapper::CompileGeometryShader(std::string& geometryShaderPath)
{
    GLuint GeometryShader;
    GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    CompileShader(geometryShaderPath, GeometryShader);
    LogShaderError(GeometryShader, "Geometry Shader compilation failed: ");

    return GeometryShader;
}

void ShaderWrapper::LogShaderError(GLuint geometryShader, const std::string& message)
{
    GLint ShaderCompilationResult;
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &ShaderCompilationResult);
    if (!ShaderCompilationResult)
    {
        char Log[512] = "Error";
        glGetShaderInfoLog(geometryShader, 512, nullptr, Log);
        SPDLOG_ERROR(message + std::string(Log));
    }
}

void ShaderWrapper::CompileShader(std::string& shaderPath, GLuint shader)
{
    std::string ShaderCode;
    LoadShader(shaderPath, ShaderCode);
    const GLchar* ConstCharPtrShaderCode = ShaderCode.c_str();
    glShaderSource(shader, 1, &ConstCharPtrShaderCode, nullptr);
    glCompileShader(shader);
}

GLint ShaderWrapper::TrySetVec4f(const std::string& name, glm::vec4 value) const
{
    GLint UniformLocation = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    return UniformLocation;
}





