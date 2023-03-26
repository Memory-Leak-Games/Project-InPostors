#include "Rendering/Assets/MaterialAsset.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Rendering/ShaderProgram.h"
#include "Rendering/Assets/ShaderAsset.h"
#include "Core/AssetManager/AssetManager.h"

#include "Macros.h"

using json = nlohmann::json;

namespace mlg {
    void MaterialAsset::FloatUniform::ApplyValue(struct ShaderProgram* shaderProgram) {
        shaderProgram->SetFloat(name, value);
    }

    void MaterialAsset::Vec4Uniform::ApplyValue(struct ShaderProgram* shaderProgram) {
        shaderProgram->SetVec4F(name, value);
    }

    void MaterialAsset::Texture::ApplyValue(struct ShaderProgram* shaderProgram) {
        glActiveTexture(GL_TEXTURE0 + index);
        shaderProgram->SetInt(name, index);
        textureAsset->Bind();
    }

    MaterialAsset::MaterialAsset(const std::string& path)
            : Asset(path) {}

    void MaterialAsset::Activate() {
        shaderProgram->Activate();
        for (const auto& uniform : uniforms) {
            uniform->ApplyValue(shaderProgram.get());
        }
    }

    void MaterialAsset::DeActivate() {
        shaderProgram->DeActivate();
    }

    MaterialAsset::~MaterialAsset() = default;

    void MaterialAsset::Load() {
        std::string path = GetPath();
        SPDLOG_DEBUG("Loading Material at path: {}", path);

        std::ifstream materialFile{path};

        json materialJson = json::parse(materialFile);

        shaderProgram = std::make_shared<ShaderProgram>(AssetManager::GetAsset<ShaderAsset>(materialJson["vertexShader"]),
                                                        AssetManager::GetAsset<ShaderAsset>(materialJson["fragmentShader"]));
        ParseUniforms(materialJson);
        ParseTextures(materialJson);
    }

    void MaterialAsset::ParseUniforms(const json& materialJson) {
        for (const auto& jsonUniform : materialJson["uniforms"]) {
            std::string type = jsonUniform["type"].get<std::string>();

            if (type == "float") {
                auto newUniform = std::make_unique<FloatUniform>();
                newUniform->name = jsonUniform["name"].get<std::string>();
                newUniform->value = jsonUniform["value"].get<float>();
                uniforms.push_back(std::move(newUniform));
            } else if (type == "vec4") {
                auto newUniform = std::make_unique<Vec4Uniform>();
                newUniform->name = jsonUniform["name"].get<std::string>();
                newUniform->value = {
                        jsonUniform["value"][0].get<float>(),
                        jsonUniform["value"][1].get<float>(),
                        jsonUniform["value"][2].get<float>(),
                        jsonUniform["value"][3].get<float>(),
                };
                uniforms.push_back(std::move(newUniform));
            } else {
                SPDLOG_ERROR("MATERIAL: Unknow uniform type {}", type);
            }
        }
    }

    void MaterialAsset::ParseTextures(const json& materialJson) {
        for (const auto& jsonTexture : materialJson["textures"]) {
            std::string path = jsonTexture["path"].get<std::string>();

            auto newUniform = std::make_unique<Texture>();
            newUniform->name = jsonTexture["name"];
            newUniform->textureAsset = AssetManager::GetAsset<TextureAsset>(path);
            newUniform->index = jsonTexture["index"].get<int>();
            uniforms.push_back(std::move(newUniform));
        }
    }

    const std::shared_ptr<ShaderProgram>& MaterialAsset::GetShaderProgram() const {
        return shaderProgram;
    }

} // mlg