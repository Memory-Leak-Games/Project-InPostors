#include "UI/ProgressBar.h"

#include <glad/glad.h>

#include "Core/Window.h"
#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/ShaderProgram.h"

#include "Core/Time.h"

namespace mlg {
    ProgressBar::ProgressBar(const std::shared_ptr<struct MaterialAsset>& material) : Image(material) {}

    // TODO: Do przemyślenia czy ta klasa powinna dziedziczyć z obrazka a jeżeli tak to wypadało by korzystać z jej kodu
    void ProgressBar::Draw() {
        material->Activate();

        // TODO: To oczywiście wyrzucić
        percentage = std::sin(Time::GetSeconds()) * 0.5 + 0.5;

        // TODO: nie liczyć tego co klatkę tylko jeżeli wielkość okna się zmieni (może trzymać to w Rendererz jako pole statyczne)
        Window* window = Window::GetInstance();
        glm::mat4 projection = glm::ortho(0.0f, (float) window->GetWidth(), 0.0f, (float) window->GetHeight());

        material->GetShaderProgram()->SetVec2F("size", GetSize());
        material->GetShaderProgram()->SetVec2F("screenPosition", GetPosition());
        material->GetShaderProgram()->SetMat4F("projection", projection);

        material->GetShaderProgram()->SetFloat("percentage", percentage);

        // TODO: Sam prostokąt też bym rysował w osobnej klasie statycznej
        DrawRect();

        material->DeActivate();
    }
} // mlg