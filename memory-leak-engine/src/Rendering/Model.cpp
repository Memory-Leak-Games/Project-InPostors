#include "Rendering/Model.h"

#include "assimp/Importer.hpp"
#include <filesystem>

#include "Core/AssetManager/AssetManager.h"
#include "include/Rendering/Assets/TextureAsset.h"

#include "Macros.h"
#include "stb_image.h"

using namespace mlg;

void Model::Draw() {
    for (const std::shared_ptr<Mesh>& Item : meshes) {
        Item->Draw();
    }
}

Model::Model(const std::string& Path)
        : modelPath(Path) {
    Assimp::Importer AssimpImporter;
    uint32_t AssimpProcessFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes;
    const aiScene* AssimpScene = AssimpImporter.ReadFile(Path, AssimpProcessFlags);

    if (!AssimpScene || AssimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !AssimpScene->mRootNode) {
        SPDLOG_ERROR("ASSIMP {}", AssimpImporter.GetErrorString());
        return;
    }

    ProcessNode(AssimpScene->mRootNode, AssimpScene);
}

void Model::ProcessNode(aiNode* NodePtr, const aiScene* ScenePtr) {
    for (uint32_t i = 0; i < NodePtr->mNumMeshes; ++i) {
        aiMesh* MeshPtr = ScenePtr->mMeshes[NodePtr->mMeshes[i]];
        meshes.push_back(ProcessMesh(MeshPtr, ScenePtr));
    }

    for (int i = 0; i < NodePtr->mNumChildren; ++i) {
        ProcessNode(NodePtr->mChildren[i], ScenePtr);
    }
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh* MeshPtr, const aiScene* ScenePtr) {
    std::vector<Vertex> Vertices;
    std::vector<GLuint> Indices;

    for (uint32_t i = 0; i < MeshPtr->mNumVertices; i++) {
        Vertices.push_back(GetVertexFromAIMesh(MeshPtr, i));
    }

    for (uint32_t i = 0; i < MeshPtr->mNumFaces; i++) {
        aiFace face = MeshPtr->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            Indices.push_back(face.mIndices[j]);
        }
    }

    return std::make_shared<Mesh>(Vertices, Indices);
}

Vertex Model::GetVertexFromAIMesh(const aiMesh* MeshPtr, unsigned int i) {
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

const std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes() const {
    return meshes;
}

