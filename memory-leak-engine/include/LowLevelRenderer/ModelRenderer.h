#pragma once

#include <glad/glad.h>

namespace mlg {
    class ModelRenderer {
    private:
        std::unordered_map<class Model*, std::set<class ModelNode*>> nodesMap;
        std::unordered_map<class Model*, GLuint> bufferMap;

    public:
        void Draw(class Core* engine);

        void AddNode(ModelNode* node);

        void RemoveNode(ModelNode* node);

        void DrawModel(Model* model, std::set<struct ModelNode*>& nodeArray, Core* engine);

        void UpdateMatrixBuffer(Model* model, std::set<struct ModelNode*>& nodeArray);
    };
}// namespace mlg
