#pragma once

#include "TextureAsset.h"
#include "nlohmann/json.hpp"

// TODO: Create more uniform structs

namespace mlg {

    class MaterialAsset : public Asset {
    private:
        struct Uniform {
            std::string name;
            virtual void ApplyValue(class ShaderProgram* shaderProgram) = 0;

            virtual ~Uniform() = default;
        };

        struct FloatUniform : public Uniform {
            float value{};
            void ApplyValue(class ShaderProgram* shaderProgram) override;
        };

        struct IntUniform : public Uniform {
            int value{};
            void ApplyValue(class ShaderProgram* shaderProgram) override;
        };

        struct Vec4Uniform : public Uniform {
            glm::vec4 value{};
            void ApplyValue(class ShaderProgram* shaderProgram) override;
        };

        struct Texture : public Uniform {
            int index;
            std::shared_ptr<TextureAsset> textureAsset;
            void ApplyValue(class ShaderProgram* shaderProgram) override;
        };

        std::shared_ptr<class ShaderProgram> shaderProgram;
        std::vector<std::unique_ptr<Uniform>> uniforms;
    public:
        explicit MaterialAsset(const std::string& path);

        void Activate();
        void DeActivate();

        void Load() override;
        ~MaterialAsset() override;

        void ParseUniforms(const nlohmann::basic_json<>& materialJson);
        void ParseTextures(const nlohmann::basic_json<>& materialJson);

        [[nodiscard]] const std::shared_ptr<struct ShaderProgram>& GetShaderProgram() const;
    };

} // mlg
