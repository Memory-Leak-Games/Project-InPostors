#pragma once

#include <glad/glad.h>

namespace mlg {
    class ShaderProgram {
    private:
        uint32_t shaderProgramId = 0;

    public:
        ShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath);
        ShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath);

        void Activate() const;
        void DeActivate() const;

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetIntArray(const std::string& name, int* values, uint32_t count) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2F(const std::string& name, glm::vec2 value) const;
        void SetVec3F(const std::string& name, glm::vec3 value) const;
        void SetVec4F(const std::string& name, glm::vec4 value) const;
        void SetMat4F(const std::string& name, glm::mat4 value) const;

        [[nodiscard]] int32_t TrySetVec4f(const std::string& name, glm::vec4 value) const;

        [[nodiscard]] int32_t GetUniformLocation(const std::string& name) const;

    private:
        static std::string LoadShader(const std::string& shaderPath);

        static void CompileShader(const std::string& shaderPath, uint32_t shader);

        static uint32_t CompileVertexShader(const std::string& vertexShaderPath);
        static uint32_t CompileFragmentShader(const std::string& fragmentShaderPath);
        static uint32_t CompileGeometryShader(const std::string& geometryShaderPath);

        void LinkProgram(uint32_t vertexShader, uint32_t fragmentShader, uint32_t geometryShader);

        static void LogShaderError(uint32_t geometryShader, const std::string& message);
    };

}

