#ifndef SOLARSYSTEM_MODELNODE_H
#define SOLARSYSTEM_MODELNODE_H

#include <memory>

#include "Node.h"

class ModelNode: public Node
{
private:
    std::shared_ptr<class Model> ModelPtr;
    class ModelRenderer* Renderer;

public:
    explicit ModelNode(std::shared_ptr<Model> ModelPtr, ModelRenderer* Renderer);

    Model* GetModel();
    virtual ~ModelNode();

protected:
    virtual void Draw(glm::mat4& ParentTransform, bool IsDirty) override;
};


#endif //SOLARSYSTEM_MODELNODE_H
