#pragma once

#include "Mesh.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace mlg {
    class Model {
    private:
        std::shared_ptr<ShaderWrapper> shader;
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::string modelPath;

    public:
        explicit Model(const std::string &Path, std::shared_ptr<ShaderWrapper> Shared);

        void Draw();

        [[nodiscard]] const std::shared_ptr<ShaderWrapper> &GetShader() const;

        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>> &GetMeshes() const;

    private:
        void ProcessNode(aiNode *NodePtr, const aiScene *ScenePtr);

        std::shared_ptr<Mesh> ProcessMesh(aiMesh *MeshPtr, const aiScene *ScenePtr);

        std::vector<Texture>
        LoadMaterialTextures(aiMaterial *Material, aiTextureType Type, const std::string &TypeName);

        static Vertex GetVertexFromAIMesh(const aiMesh *MeshPtr, unsigned int i);

        GLuint TextureFromFile(const std::string &Path);
    };
}
