#pragma once

#include <map>
#include <set>
#include "glad/glad.h"

class ModelRenderer
{
private:
    std::map<class Model*, std::set<class ModelNode*>> nodesMap;
    std::map<class Model*, GLuint> bufferMap;
public:
    void Draw(class MainEngine* engine);

    void AddNode(ModelNode* node);
    void RemoveNode(ModelNode* node);
    void DrawModel(Model* model, std::set<struct ModelNode*>& nodeArray, MainEngine* engine);
    void UpdateMatrixBuffer(Model* model, std::set<struct ModelNode*>& nodeArray);
};
