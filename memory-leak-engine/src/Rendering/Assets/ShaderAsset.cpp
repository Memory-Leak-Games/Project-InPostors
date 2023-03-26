#include "Rendering/Assets/ShaderAsset.h"

#include "glad/glad.h"
#include <fstream>

#include "Macros.h"

namespace mlg {
    ShaderAsset::ShaderAsset(const std::string& path) : Asset(path) {}

    void ShaderAsset::Load() {
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
            MLG_ASSERT(false, "Wrong shader type");
        }
    }

    GLuint ShaderAsset::CompileFragmentShader(const std::string& fragmentShaderPath) {
        GLuint FragmentShader;
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        CompileShader(fragmentShaderPath, FragmentShader);
        LogShaderError(FragmentShader, "Fragment Shader compilation failed: ");

        return FragmentShader;
    }

    GLuint ShaderAsset::CompileVertexShader(const std::string& vertexShaderPath) {
        GLuint VertexShader;
        VertexShader = glCreateShader(GL_VERTEX_SHADER);

        CompileShader(vertexShaderPath, VertexShader);
        LogShaderError(VertexShader, "Vertex Shader compilation failed: ");

        return VertexShader;
    }

    GLuint ShaderAsset::CompileGeometryShader(const std::string& geometryShaderPath) {
        GLuint GeometryShader;
        GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        CompileShader(geometryShaderPath, GeometryShader);
        LogShaderError(GeometryShader, "Geometry Shader compilation failed: ");

        return GeometryShader;
    }

    void ShaderAsset::CompileShader(const std::string& shaderCode, GLuint shader) {
        std::string ShaderCode = shaderCode;
        const GLchar* ConstCharPtrShaderCode = ShaderCode.c_str();
        glShaderSource(shader, 1, &ConstCharPtrShaderCode, nullptr);
        glCompileShader(shader);
    }

    void ShaderAsset::LogShaderError(GLuint shader, const std::string& message) {
        GLint ShaderCompilationResult;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ShaderCompilationResult);
        if (!ShaderCompilationResult) {
            char Log[512] = "Error";
            glGetShaderInfoLog(shader, 512, nullptr, Log);
            SPDLOG_ERROR(message + std::string(Log));
        }
    }

    uint32_t ShaderAsset::GetShaderID() {
        return shaderID;
    }

    ShaderAsset::~ShaderAsset() {
        glDeleteShader(shaderID);
    }
}// mlg
