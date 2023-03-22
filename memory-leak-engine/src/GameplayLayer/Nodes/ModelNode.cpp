#include "GameplayLayer/Nodes/Node.h"
#include "GameplayLayer/Nodes/ModelNode.h"
#include "Rendering/Model.h"

using namespace mlg;

ModelNode::ModelNode(std::shared_ptr<struct Model> ModelPtr)
        : Node(), ModelPtr(ModelPtr) {
}

void ModelNode::Draw(glm::mat4 &ParentTransform, bool IsDirty) {
    Node::Draw(ParentTransform, IsDirty);

    ModelPtr->GetShader()->Activate();
    ModelPtr->GetShader()->SetMat4F("World", *GetWorldTransformMatrix());
    ModelPtr->Draw();
}

Model *ModelNode::GetModel() {
    return ModelPtr.get();
}

ModelNode::~ModelNode() {
}

