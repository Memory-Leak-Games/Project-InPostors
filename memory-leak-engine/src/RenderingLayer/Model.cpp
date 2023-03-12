#include "RenderingLayer/Model.h"

#include "assimp/Importer.hpp"
#include <filesystem>

#include "LoggingMacros.h"
#include "stb_image.h"

using namespace mlg;

void Model::Draw() {
    for (std::shared_ptr<Mesh> Item: meshes) {
        Item->Draw(*GetShader());
    }
}

Model::Model(const std::string &Path, std::shared_ptr<ShaderWrapper> Shader)
        : modelPath(Path), shader(Shader) {
    Assimp::Importer AssimpImporter;

    uint32_t AssimpProcessFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes;

    const aiScene *AssimpScene = AssimpImporter.ReadFile(Path, AssimpProcessFlags);

    if (!AssimpScene || AssimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !AssimpScene->mRootNode) {
        SPDLOG_ERROR("ASSIMP {}", AssimpImporter.GetErrorString());
        return;
    }

    ProcessNode(AssimpScene->mRootNode, AssimpScene);
}

void Model::ProcessNode(aiNode *NodePtr, const aiScene *ScenePtr) {
    for (uint32_t i = 0; i < NodePtr->mNumMeshes; ++i) {
        aiMesh *MeshPtr = ScenePtr->mMeshes[NodePtr->mMeshes[i]];
        meshes.push_back(ProcessMesh(MeshPtr, ScenePtr));
    }

    for (int i = 0; i < NodePtr->mNumChildren; ++i) {
        ProcessNode(NodePtr->mChildren[i], ScenePtr);
    }
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh *MeshPtr, const aiScene *ScenePtr) {
    std::vector<Vertex> Vertices;
    std::vector<GLuint> Indices;
    std::vector<Texture> Textures;

    for (uint32_t i = 0; i < MeshPtr->mNumVertices; i++) {
        Vertices.push_back(GetVertexFromAIMesh(MeshPtr, i));
    }

    for (uint32_t i = 0; i < MeshPtr->mNumFaces; i++) {
        aiFace face = MeshPtr->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            Indices.push_back(face.mIndices[j]);
        }
    }

    if (MeshPtr->mMaterialIndex >= 0) {
        aiMaterial *Material = ScenePtr->mMaterials[MeshPtr->mMaterialIndex];

        std::vector<Texture> DiffuseMaps = LoadMaterialTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse");
        Textures.insert(Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end());

        std::vector<Texture> SpecularMaps = LoadMaterialTextures(Material, aiTextureType_SPECULAR, "texture_specular");
        Textures.insert(Textures.end(), SpecularMaps.begin(), SpecularMaps.end());

        std::vector<Texture> NormalMaps = LoadMaterialTextures(Material, aiTextureType_HEIGHT, "texture_normalmap");
        Textures.insert(Textures.end(), NormalMaps.begin(), NormalMaps.end());
    }

    return std::make_shared<Mesh>(Vertices, Indices, Textures);
}

Vertex Model::GetVertexFromAIMesh(const aiMesh *MeshPtr, unsigned int i) {
    Vertex NewVertex{};

    glm::vec3 Position, Normal;
    glm::vec2 TextureCoords;

    Position.x = MeshPtr->mVertices[i].x;
    Position.y = MeshPtr->mVertices[i].y;
    Position.z = MeshPtr->mVertices[i].z;

    Normal.x = MeshPtr->mNormals[i].x;
    Normal.y = MeshPtr->mNormals[i].y;
    Normal.z = MeshPtr->mNormals[i].z;

    if (MeshPtr->mTextureCoords[0]) {
        TextureCoords.x = MeshPtr->mTextureCoords[0][i].x;
        TextureCoords.y = MeshPtr->mTextureCoords[0][i].y;
    } else {
        TextureCoords = glm::vec2(0.0f, 0.0f);
    }

    NewVertex.position = Position;
    NewVertex.normal = Normal;
    NewVertex.texCoord = TextureCoords;

    return NewVertex;
}

std::vector<Texture>
Model::LoadMaterialTextures(aiMaterial *Material, aiTextureType Type, const std::string &TypeName) {
    std::vector<Texture> Textures;
    for (uint32_t i = 0; i < Material->GetTextureCount(Type); i++) {
        aiString Path;
        Material->GetTexture(Type, i, &Path);
        Texture Texture;
        Texture.id = TextureFromFile(Path.C_Str());
        Texture.textureType = TypeName;
        Texture.texturePath = Path.C_Str();
        Textures.push_back(Texture);
    }
    return Textures;
}

GLuint Model::TextureFromFile(const std::string &Path) {

    unsigned int TextureID;
    glGenTextures(1, &TextureID);

    std::filesystem::path PathFromExecutable = std::filesystem::path{modelPath}.parent_path() / Path;
    SPDLOG_DEBUG("Loading texture at path: {}", PathFromExecutable.string());

    int Width, Height, NumberOfComponents;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *ImageData = stbi_load(PathFromExecutable.string().c_str(), &Width, &Height, &NumberOfComponents, 0);
    if (ImageData) {
        GLenum ColorFormat;
        if (NumberOfComponents == 1)
            ColorFormat = (GL_RED);
        else if (NumberOfComponents == 3)
            ColorFormat = (GL_RGB);
        else if (NumberOfComponents == 4)
            ColorFormat = (GL_RGBA);

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, ColorFormat, Width, Height, 0, ColorFormat, GL_UNSIGNED_BYTE, ImageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(ImageData);
    } else {
        SPDLOG_ERROR("Failed to load texture at path: {}", PathFromExecutable.string());
        stbi_image_free(ImageData);
    }

    return TextureID;
}

const std::shared_ptr<ShaderWrapper> &Model::GetShader() const {
    return shader;
}

const std::vector<std::shared_ptr<Mesh>> &Model::GetMeshes() const {
    return meshes;
}

