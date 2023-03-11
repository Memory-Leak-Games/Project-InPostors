#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace mlg {
    class ShaderWrapper {
    private:
        GLuint shaderProgramId = -1;

    public:
        ShaderWrapper(std::string vertexShaderPath, std::string fragmentShaderPath);

        ShaderWrapper(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath);

        void Activate() const;

        void SetBool(const std::string &name, bool value) const;

        void SetInt(const std::string &name, int value) const;

        void SetFloat(const std::string &name, float value) const;

        void SetVec4F(const std::string &name, glm::vec4 value) const;

        void SetMat4F(const std::string &name, glm::mat4 value) const;

        [[nodiscard]] GLint GetUniformLocation(const std::string &name) const;

        GLint TrySetVec4f(const std::string &name, glm::vec4 value) const;

        GLuint GetShaderProgramId() const;

    private:
        static void LoadShader(std::string &shaderPath, std::string &shaderCodeOut);

        static GLuint CompileVertexShader(std::string &vertexShaderPath);

        static GLuint CompileFragmentShader(std::string &fragmentShaderPath);

        static GLuint CompileGeometryShader(std::string &geometryShaderPath);

        void LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader);

        static void CompileShader(std::string &shaderPath, GLuint shader);

        static void LogShaderError(GLuint geometryShader, const std::string &message);
    };

}

