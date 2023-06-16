#include "Core/SceneManager/LoadingScreen.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/Window.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Primitives/Rect.h"
#include "Rendering/QuadRenderer.h"
#include "Rendering/ShaderProgram.h"
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>

#define LOADING_IMAGE_SIZE 256

mlg::LoadingScreen::LoadingScreen()
    : backgroundQuad(std::make_unique<QuadRenderer>()),
      loadingImage(std::make_unique<Rect>()) {
    backgroundQuad->Initialize();

    backgroundMaterial = AssetManager::GetAsset<MaterialAsset>(
            "res/materials/ui/loading/loading_background_material.json");
    imageMaterial = AssetManager::GetAsset<MaterialAsset>(
            "res/materials/ui/loading/loading_label_material.json");

    imageMaterial->Activate();
    imageMaterial->GetShaderProgram()->SetVec2F(
            "size", glm::vec2(LOADING_IMAGE_SIZE));
}

mlg::LoadingScreen::~LoadingScreen() = default;

void mlg::LoadingScreen::Draw() {
    int32_t windowWidth = Window::Get()->GetWidth();
    int32_t windowHeight = Window::Get()->GetHeight();

    // put image in left down corner
    glm::vec2 position = {
            windowWidth - LOADING_IMAGE_SIZE,
            LOADING_IMAGE_SIZE};

    glm::mat4 projection = glm::ortho(
            0.0f, (float) windowWidth,
            0.0f, (float) windowHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    backgroundMaterial->Activate();
    backgroundQuad->Draw();

    imageMaterial->Activate();
    imageMaterial->GetShaderProgram()->SetVec2F("screenPosition", position);
    imageMaterial->GetShaderProgram()->SetMat4F("projection", projection);
    loadingImage->Draw();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    Window::Get()->SwapBuffers();
}
