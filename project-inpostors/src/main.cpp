// This is not allowed in Game layer
#include "Rendering/Camera.h"
#include "Rendering/Model.h"
#include "Rendering/Renderer.h"
#include <Rendering/RenderingAPI.h>

#include <Rendering/Assets/MaterialAsset.h>
#include <Rendering/Assets/ModelAsset.h>

#include <Core/HID/Input.h>
#include <Core/AssetManager/AssetManager.h>
#include "Core/Core.h"
#include "Core/Time.h"

#include "SceneGraph/SceneGraph.h"

#include <Gameplay/EntityManager.h>
#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/StaticMeshComponent.h>

class ComponentTest : public mlg::Component {
public:
    ComponentTest(const std::weak_ptr<mlg::Entity>& owner, const std::string& name) : Component(owner, name) {}

    void Update() override {
        glm::vec3 position = GetOwner().lock()->GetTransform().GetPosition();
        position.z = glm::sin(mlg::Time::GetSeconds() * 2.f) * 8.f;

        GetOwner().lock()->GetTransform().SetRotation({{0.f, mlg::Time::GetSeconds(), 0.f}});
        GetOwner().lock()->GetTransform().SetPosition(position);

        if (mlg::Input::IsActionPressed("test_button")) {
            GetOwner().lock()->QueueForDeletion();
        }
    }

    ~ComponentTest() override {

    }
};

class ProjectInpostors {
private:
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Time::Initialize();
        mlg::Window::Initialize("Memory Leak Engine", 1280, 720);
        mlg::Window::GetInstance()->SetVerticalSync(false);
        mlg::RenderingAPI::Initialize();
        mlg::Renderer::Initialize();
        mlg::AssetManager::Initialize();
        mlg::SceneGraph::Initialize();
        mlg::ComponentManager::Initialize();
        mlg::EntityManager::Initialize();

        mlg::Core::Initialize();
        mlg::Input::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

        mlg::EntityManager::Stop();
        mlg::ComponentManager::Stop();
        mlg::SceneGraph::Stop();
        mlg::Input::Stop();
        mlg::Core::Stop();
        mlg::AssetManager::Stop();
        mlg::Renderer::Stop();
        mlg::RenderingAPI::Stop();
        mlg::Window::Stop();
        mlg::Time::Stop();

        return 0;
    }

    void PrepareScene() {
        mlg::Camera::GetInstance()->SetPosition({0, 0, -20});
        auto tardisMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Tardis/tardis_material.json");
        auto tardisModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Tardis/tardis.obj");

        auto tardisEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("TardisOne", true, mlg::SceneGraph::GetRoot());
        tardisEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", tardisModel, tardisMaterial);
        tardisEntity.lock()->AddComponent<ComponentTest>("RotationComponent");

        auto tardisMaterial1 = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Tardis/tardis_material_green.json");
        auto tardisModel1 = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Tardis/tardis.obj");

        auto tardisEntity1 = mlg::EntityManager::SpawnEntity<mlg::Entity>("TardisRight", true, mlg::SceneGraph::GetRoot());
        tardisEntity1.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", tardisModel1, tardisMaterial1);
        tardisEntity1.lock()->AddComponent<ComponentTest>("RotationComponent");

        tardisEntity1.lock()->GetTransform().SetPosition({-7.f, 0.f, 0.f});

        auto tardisMaterial2 = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Tardis/tardis_material_another.json");
        auto tardisModel2 = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Tardis/tardis.obj");

        auto tardisEntity2 = mlg::EntityManager::SpawnEntity<mlg::Entity>("TardisLeft", true, mlg::SceneGraph::GetRoot());
        tardisEntity2.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", tardisModel2, tardisMaterial2);
        tardisEntity2.lock()->AddComponent<ComponentTest>("RotationComponent");

        tardisEntity2.lock()->GetTransform().SetPosition({7.f, 0.f, 0.f});
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();
    std::srand(std::time(0));

    ProjectInpostors game;
    return game.Main(argc, argv);
}
