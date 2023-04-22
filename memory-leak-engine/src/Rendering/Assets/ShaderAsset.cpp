#include "Rendering/Assets/ShaderAsset.h"

#include "glad/glad.h"
#include <fstream>

#include "Macros.h"

namespace mlg {
    ShaderAsset::ShaderAsset(const std::string& path) : Asset(path) {}

    void ShaderAsset::Load() {
        ZoneScopedN("Load ShaderAsset");
        std::string path = GetPath();
        SPDLOG_DEBUG("Loading shader at path: {}", path);

        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        shaderFile.open(path);
        std::stringstream ShaderStream;

        ShaderStream << shaderFile.rdbuf();
        shaderFile.close();

        std::string type = path.substr(path.find_last_of('.') + 1);

        if (type == "vert") {
            shaderID = CompileVertexShader(ShaderStream.str());
        } else if (type == "frag") {
            shaderID = CompileFragmentShader(ShaderStream.str());
        } else if (type == "geom") {
            shaderID = CompileGeometryShader(ShaderStream.str());
        } else {
            MLG_ASSERT_MSG(false, "Wrong shader type");
        }
    }

    uint32_t ShaderAsset::CompileFragmentShader(const std::string& fragmentShaderPath) {
        uint32_t FragmentShader;
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        CompileShader(fragmentShaderPath, FragmentShader);
        LogShaderError(FragmentShader, "Fragment Shader compilation failed: ");

        return FragmentShader;
    }

    uint32_t ShaderAsset::CompileVertexShader(const std::string& vertexShaderPath) {
        uint32_t VertexShader;
        VertexShader = glCreateShader(GL_VERTEX_SHADER);

        CompileShader(vertexShaderPath, VertexShader);
        LogShaderError(VertexShader, "Vertex Shader compilation failed: ");

        return VertexShader;
    }

    uint32_t ShaderAsset::CompileGeometryShader(const std::string& geometryShaderPath) {
        uint32_t GeometryShader;
        GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        CompileShader(geometryShaderPath, GeometryShader);
        LogShaderError(GeometryShader, "Geometry Shader compilation failed: ");

        return GeometryShader;
    }

    void ShaderAsset::CompileShader(const std::string& shaderCode, uint32_t shader) {
        std::string ShaderCode = shaderCode;
        const GLchar* ConstCharPtrShaderCode = ShaderCode.c_str();
        glShaderSource(shader, 1, &ConstCharPtrShaderCode, nullptr);
        glCompileShader(shader);
    }

    void ShaderAsset::LogShaderError(uint32_t shader, const std::string& message) {
        GLint ShaderCompilationResult;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ShaderCompilationResult);
        if (!ShaderCompilationResult) {
            char Log[512] = "Error";
            glGetShaderInfoLog(shader, 512, nullptr, Log);
            SPDLOG_ERROR(message + "\n" + std::string(Log));
        }
    }

    uint32_t ShaderAsset::GetShaderID() {
        return shaderID;
    }

    ShaderAsset::~ShaderAsset() {
        glDeleteShader(shaderID);
    }
}// mlg
