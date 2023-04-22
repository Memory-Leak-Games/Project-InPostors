#pragma once

#include <glad/glad.h>

namespace mlg {
    class ShaderProgram {
    private:
        uint32_t shaderProgramId = 0;
        std::shared_ptr<class ShaderAsset> vertexShader;
        std::shared_ptr<class ShaderAsset> fragmentShader;
        std::shared_ptr<class ShaderAsset> geometryShader;

        mutable std::unordered_map<std::string, int32_t> uniformCache;

    public:
        ShaderProgram(const std::shared_ptr<ShaderAsset>& vertexShader,
                      const std::shared_ptr<ShaderAsset>& fragmentShader);
        ShaderProgram(const std::shared_ptr<ShaderAsset>& vertexShader,
                      const std::shared_ptr<ShaderAsset>& fragmentShader,
                      const std::shared_ptr<ShaderAsset>& geometryShader);

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
        void SetMat3F(const std::string& name, glm::mat3 value) const;

        [[nodiscard]] int32_t TrySetVec4f(const std::string& name, glm::vec4 value) const;

        [[nodiscard]] int32_t GetUniformLocation(const std::string& name) const;

    private:
        void LinkProgram(uint32_t vShader, uint32_t fShader, uint32_t gShader);
    };

}

