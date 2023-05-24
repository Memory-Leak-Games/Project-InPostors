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

    // TODO: Do przemyślenia czy ta klasa powinna dziedziczyć z obrazka a jeżeli tak to wypadało by korzystać z jej kodu
    void ProgressBar::Draw(const UIRenderer* renderer) {
        ZoneScopedN("Draw ProgressBar");
        UIComponent::Draw(renderer);

        material->Activate();

        // TODO: To oczywiście wyrzucić
        percentage += Time::GetDeltaSeconds() * 0.5;
        if (percentage > 1.f){
            percentage = Random::get(-5.0f, 0.f);
        }

        material->GetShaderProgram()->SetVec2F("size", GetSize() * renderer->uiScale);
        material->GetShaderProgram()->SetVec2F("screenPosition", actualPosition);
        material->GetShaderProgram()->SetMat4F("projection", renderer->GetProjection());

        material->GetShaderProgram()->SetFloat("percentage", percentage);

        // TODO: Sam prostokąt też bym rysował w osobnej klasie statycznej
        DrawRect();

        material->DeActivate();
    }
} // mlg