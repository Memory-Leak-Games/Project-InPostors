#pragma once

#include <glad/glad.h>

namespace mlg {
    class ShaderWrapper {
    private:
        GLuint shaderProgramId = 0;

    public:
        ShaderWrapper(std::string vertexShaderPath, std::string fragmentShaderPath);

        ShaderWrapper(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath);

        void Activate() const;
        void DeActivate() const;

        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetIntArray(const std::string &name, int *values, uint32_t count) const;
        void SetFloat(const std::string &name, float value) const;
        void SetVec2F(const std::string &name, glm::vec2 value) const;
        void SetVec3F(const std::string &name, glm::vec3 value) const;
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

