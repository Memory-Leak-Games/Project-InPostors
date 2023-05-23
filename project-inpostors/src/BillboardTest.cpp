#include "BillboardTest.h"

#include "Rendering/Assets/MaterialAsset.h"
#include "Gameplay/Entity.h"
#include "UI/Components/Image.h"
#include "SceneGraph/Transform.h"
#include "Rendering/Camera.h"
#include "Rendering/CommonUniformBuffer.h"
#include "UI/UIRenderer.h"

BillboardTest::BillboardTest(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, const std::shared_ptr<mlg::MaterialAsset>& material)
    : Image(owner, name, material) {}

void BillboardTest::Draw(const mlg::UIRenderer* renderer) {
    glm::vec4 world = glm::vec4(player.lock()->GetTransform().GetWorldPosition(), 1.0f);
    auto projection =  mlg::CommonUniformBuffer::GetUniforms().projection;
    auto view =  mlg::CommonUniformBuffer::GetUniforms().view;
    actualPosition = projection * view * world;
    actualPosition += glm::vec2(1, 1);
    actualPosition.x *= renderer->windowWidth * 0.5;
    actualPosition.y *= renderer->windowHeight * 0.5;
    actualPosition.y += 48.f;
    Image::Draw(renderer);
    //std::string s = std::to_string(actualPosition[0]) + " " + std::to_string(actualPosition[1]) + " " + std::to_string(actualPosition[2]);
    //SPDLOG_DEBUG(s);
}
