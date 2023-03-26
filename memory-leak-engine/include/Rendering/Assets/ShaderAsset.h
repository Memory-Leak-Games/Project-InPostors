#pragma once

#include "Core/AssetManager/Asset.h"
#include <glad/glad.h>

namespace mlg {

    class ShaderAsset : public Asset {
        GLuint shaderID = 0;
    public:
        explicit ShaderAsset(const std::string& path);

        void Load() override;

        static uint32_t CompileVertexShader(const std::string& vertexShaderPath);
        static uint32_t CompileFragmentShader(const std::string& fragmentShaderPath);
        static uint32_t CompileGeometryShader(const std::string& geometryShaderPath);

        static void CompileShader(const std::string& shaderCode, uint32_t shader);
        static void LogShaderError(uint32_t shader, const std::string& message);

        uint32_t GetShaderID();

        ~ShaderAsset() override;
    };
}// mlg
