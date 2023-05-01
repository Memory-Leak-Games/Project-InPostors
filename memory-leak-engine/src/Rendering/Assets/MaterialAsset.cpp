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

    std::unique_ptr<MaterialAsset::Uniform> MaterialAsset::FloatUniform::Clone() {
        auto clone = std::make_unique<MaterialAsset::FloatUniform>();
        clone->name = name;
        clone->value = value;

        return std::move(clone);
    }

    void MaterialAsset::IntUniform::ApplyValue(struct ShaderProgram* shaderProgram) {
        shaderProgram->SetInt(name, value);
    }

    std::unique_ptr<MaterialAsset::Uniform> MaterialAsset::IntUniform::Clone() {
        auto clone = std::make_unique<MaterialAsset::IntUniform>();
        clone->name = name;
        clone->value = value;

        return std::move(clone);
    }

    void MaterialAsset::Vec4Uniform::ApplyValue(struct ShaderProgram* shaderProgram) {
        shaderProgram->SetVec4F(name, value);
    }

    std::unique_ptr<MaterialAsset::Uniform> MaterialAsset::Vec4Uniform::Clone() {
        auto clone = std::make_unique<MaterialAsset::Vec4Uniform>();
        clone->name = name;
        clone->value = value;

        return std::move(clone);
    }

    void MaterialAsset::Texture::ApplyValue(struct ShaderProgram* shaderProgram) {
        glActiveTexture(GL_TEXTURE0 + index);
        shaderProgram->SetInt(name, index);
        textureAsset->Bind();
    }

    std::unique_ptr<MaterialAsset::Uniform> MaterialAsset::Texture::Clone() {
        auto clone = std::make_unique<MaterialAsset::Texture>();
        clone->name = name;
        clone->textureAsset = textureAsset;
        clone->index = index;

        return std::move(clone);
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

    MaterialAsset::~MaterialAsset() {
        uniforms.clear();
    }

    void MaterialAsset::Load() {
        ZoneScopedN("Load MaterialAsset");
        std::string path = GetPath();
        SPDLOG_DEBUG("Loading Material at path: {}", path);

        std::ifstream materialFile{path};

        json materialJson = json::parse(materialFile);

        MLG_ASSERT_MSG(materialJson.find("vertexShader") != materialJson.end(), "Material Loading Error");
        MLG_ASSERT_MSG(materialJson.find("fragmentShader") != materialJson.end(), "Material Loading Error");

        shaderProgram = std::make_shared<ShaderProgram>(
                AssetManager::GetAsset<ShaderAsset>(materialJson["vertexShader"]),
                AssetManager::GetAsset<ShaderAsset>(materialJson["fragmentShader"]));
        ParseUniforms(materialJson);
        ParseTextures(materialJson);
    }

    void MaterialAsset::ParseUniforms(const json& materialJson) {
        for (const auto& jsonUniform : materialJson["uniforms"]) {
            std::string type = jsonUniform["type"].get<std::string>();

            if (type == "float") {
                SetFloat(jsonUniform["name"], jsonUniform["value"]);
            } else if (type == "int") {
                SetInt(jsonUniform["name"], jsonUniform["value"]);
            } else if (type == "vec4") {
                SetVec4(jsonUniform["name"], {
                    jsonUniform["value"][0].get<float>(),
                    jsonUniform["value"][1].get<float>(),
                    jsonUniform["value"][2].get<float>(),
                    jsonUniform["value"][3].get<float>(),
                });
            } else {
                SPDLOG_ERROR("MATERIAL: Unknown uniform type {}", type);
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

    void MaterialAsset::RemoveUniform(const std::string& name) {
        uniforms.erase(std::remove_if(uniforms.begin(), uniforms.end(),
                       [name](const std::unique_ptr<Uniform>& uniform) { return uniform->name == name; }),
                       uniforms.end());
    }

    void MaterialAsset::SetFloat(const std::string& name, float value) {
        if (!shaderProgram->IsUniformExist(name)) {
            SPDLOG_WARN("Uniform: " + name + " not found");
            return;
        }

        RemoveUniform(name);

        auto newUniform = std::make_unique<FloatUniform>();
        newUniform->name = name;
        newUniform->value = value;
        uniforms.push_back(std::move(newUniform));
    }

    void MaterialAsset::SetInt(const std::string& name, int value) {
        if (!shaderProgram->IsUniformExist(name)) {
            SPDLOG_WARN("Uniform: " + name + " not found");
            return;
        }

        RemoveUniform(name);

        auto newUniform = std::make_unique<IntUniform>();
        newUniform->name = name;
        newUniform->value = value;
        uniforms.push_back(std::move(newUniform));
    }

    void MaterialAsset::SetVec4(const std::string& name, glm::vec4 value) {
        if (!shaderProgram->IsUniformExist(name)) {
            SPDLOG_WARN("Uniform: " + name + " not found");
            return;
        }

        RemoveUniform(name);

        auto newUniform = std::make_unique<Vec4Uniform>();
        newUniform->name = name;
        newUniform->value = value;
        uniforms.push_back(std::move(newUniform));
    }

    std::shared_ptr<MaterialAsset> MaterialAsset::CreateDynamicInstance() {
        auto dynamicInstance = std::make_shared<MaterialAsset>(GetPath());
        for (auto& uniform: uniforms) {
            dynamicInstance->uniforms.push_back(std::move(uniform->Clone()));
        }

        dynamicInstance->shaderProgram = shaderProgram;

        return dynamicInstance;
    }

} // mlg