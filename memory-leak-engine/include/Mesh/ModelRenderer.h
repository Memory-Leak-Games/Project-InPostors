#pragma once

#include <glad/glad.h>

namespace mlg {
    class ModelRenderer {
    private:
        std::map<class Model *, std::set<class ModelNode *>> nodesMap;
        std::map<class Model *, GLuint> bufferMap;
    public:
        void Draw(class CoreEngine *engine);

        void AddNode(ModelNode *node);

        void RemoveNode(ModelNode *node);

        void DrawModel(Model *model, std::set<struct ModelNode *> &nodeArray, CoreEngine *engine);

        void UpdateMatrixBuffer(Model *model, std::set<struct ModelNode *> &nodeArray);
    };
}
