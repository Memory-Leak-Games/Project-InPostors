#pragma once

#include "TextureAsset.h"
#include "nlohmann/json.hpp"
#include <glm/fwd.hpp>
#include <memory>

namespace mlg {

    class MaterialAsset : public Asset {
    private:
        struct Uniform {
            std::string name;
            virtual void ApplyValue(class ShaderProgram* shaderProgram) = 0;
            virtual std::unique_ptr<Uniform> Clone() = 0;

            virtual ~Uniform() = default;
        };

        struct FloatUniform : public Uniform {
            float value{};
            void ApplyValue(class ShaderProgram* shaderProgram) override;
            std::unique_ptr<Uniform> Clone() override;
        };

        struct IntUniform : public Uniform {
            int value{};
            void ApplyValue(class ShaderProgram* shaderProgram) override;
            std::unique_ptr<Uniform> Clone() override;
        };

        struct Vec4Uniform : public Uniform {
            glm::vec4 value{};
            void ApplyValue(class ShaderProgram* shaderProgram) override;

            std::unique_ptr<Uniform> Clone() override;
        };

        struct Texture : public Uniform {
            int index;
            std::shared_ptr<TextureAsset> textureAsset;
            void ApplyValue(class ShaderProgram* shaderProgram) override;

            std::unique_ptr<Uniform> Clone() override;
        };

        std::shared_ptr<class ShaderProgram> shaderProgram;
        std::vector<std::unique_ptr<Uniform>> uniforms;

    public:
        explicit MaterialAsset(const std::string& path);

        void Activate();
        void DeActivate();

        void Load() override;
        ~MaterialAsset() override;

        void RemoveUniform(const std::string& name);

        void SetFloat(const std::string& name, float value);
        void SetInt(const std::string& name, int value);
        void SetVec4(const std::string& name, glm::vec4 value);
        void SetTexture(
                const std::string& name,
                const std::shared_ptr<TextureAsset>& texture);

        void ParseUniforms(const nlohmann::basic_json<>& materialJson);
        void ParseTextures(const nlohmann::basic_json<>& materialJson);

        [[nodiscard]] const std::shared_ptr<struct ShaderProgram>& GetShaderProgram() const;

        std::shared_ptr<MaterialAsset> CreateDynamicInstance();

        glm::vec4 HTMLtoVec4(const std::string& html);
    };

}// namespace mlg
