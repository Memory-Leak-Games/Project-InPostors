#pragma once

#include "Mesh.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace mlg {
    class Model {
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::string modelPath;

    public:
        explicit Model(const std::string &Path);

        void Draw();

        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>> &GetMeshes() const;
    private:
        void ProcessNode(aiNode *NodePtr, const aiScene *ScenePtr);
        std::shared_ptr<Mesh> ProcessMesh(aiMesh *MeshPtr, const aiScene *ScenePtr);

        static Vertex GetVertexFromAIMesh(const aiMesh *MeshPtr, unsigned int i);
    };
}
