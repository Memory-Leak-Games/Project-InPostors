#pragma once

#include "Node.h"

namespace mlg {
    class ModelNode : public Node {
    private:
        std::shared_ptr<class Model> ModelPtr;

        class ModelRenderer *Renderer;

    public:
        explicit ModelNode(std::shared_ptr<Model> ModelPtr, ModelRenderer *Renderer);

        Model *GetModel();

        virtual ~ModelNode();

    protected:
        virtual void Draw(glm::mat4 &ParentTransform, bool IsDirty) override;
    };
}
