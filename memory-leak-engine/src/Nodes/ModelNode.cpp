#include <memory>

#include "Nodes/Node.h"
#include "Nodes/ModelNode.h"
#include "include/Mesh/Model.h"
#include "include/Mesh/ModelRenderer.h"

ModelNode::ModelNode(std::shared_ptr<struct Model> ModelPtr, ModelRenderer* Renderer)
        : Node(), ModelPtr(ModelPtr), Renderer(Renderer)
{
    Renderer->AddNode(this);
}

void ModelNode::Draw(glm::mat4& ParentTransform, bool IsDirty)
{
    Node::Draw(ParentTransform, IsDirty);

    if (Renderer)
        return;

    ModelPtr->GetShader()->Activate();
    ModelPtr->GetShader()->SetMat4F("Transform", *GetWorldTransformMatrix());
    ModelPtr->Draw();
}

Model* ModelNode::GetModel()
{
    return ModelPtr.get();
}

ModelNode::~ModelNode()
{
    if (Renderer)
        Renderer->RemoveNode(this);
}

