#include "include/UI/Components/ProgressBar.h"

#include <utility>

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"

#include "Core/Time.h"
#include "UI/UIRenderer.h"

using Random = effolkronium::random_static;

namespace mlg {
    ProgressBar::ProgressBar(std::weak_ptr<Entity> owner, std::string name, const std::shared_ptr<struct MaterialAsset>& material)
        : Image(std::move(owner), std::move(name), material) {}

    void ProgressBar::Draw(const UIRenderer* renderer) {
        ZoneScopedN("Draw ProgressBar");
        UIComponent::Draw(renderer);
        if(!visible)
            return;

        material->Activate();

        material->GetShaderProgram()->SetVec2F("size", GetSize() * renderer->uiScale);
        material->GetShaderProgram()->SetVec2F("screenPosition", actualPosition);
        material->GetShaderProgram()->SetMat4F("projection", renderer->GetProjection());

        material->GetShaderProgram()->SetFloat("percentage", percentage);

        MLG_ASSERT(UIRenderer::GetInstance()->vao != 0);
        glBindVertexArray(UIRenderer::GetInstance()->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        material->DeActivate();
    }
} // mlg