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
#include "SimplePlayer.h"

#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/StaticMeshComponent.h>
#include <Gameplay/EntityManager.h>
#include <Rendering/CommonUniformBuffer.h>
#include <Rendering/Gizmos/Gizmos.h>
#include <Physics/Physics.h>
#include <Core/Math.h>
#include <Gameplay/Components/RigidbodyComponent.h>

#include <UI/Assets/FontAsset.h>
#include <UI/Label.h>
#include <UI/Image.h>
#include <UI/Renderer2D.h>
#include <UI/ProgressBar.h>

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
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Time::Initialize();
        mlg::Window::Initialize("Memory Leak Engine", 1280, 720);
        mlg::RenderingAPI::Initialize();
        mlg::Renderer::Initialize();
        mlg::Renderer2D::Initialize();
        mlg::AssetManager::Initialize();
        mlg::Gizmos::Initialize();
        mlg::CommonUniformBuffer::Initialize();
        mlg::SceneGraph::Initialize();

        mlg::Physics::Initialize();

        mlg::ComponentManager::Initialize();
        mlg::EntityManager::Initialize();

        mlg::Core::Initialize();
        mlg::Input::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

        mlg::EntityManager::Stop();
        mlg::ComponentManager::Stop();

        mlg::Physics::Stop();

        mlg::SceneGraph::Stop();
        mlg::Input::Stop();
        mlg::Core::Stop();
        mlg::Gizmos::Stop();
        mlg::AssetManager::Stop();
        mlg::Renderer2D::Stop();
        mlg::Renderer::Stop();
        mlg::RenderingAPI::Stop();
        mlg::Window::Stop();
        mlg::Time::Stop();

        return 0;
    }

    std::shared_ptr<mlg::Label> label;
    std::shared_ptr<mlg::Image> image;
    std::shared_ptr<mlg::ProgressBar> progressBar;

    void PrepareScene() {
        mlg::Camera::GetInstance()->SetPosition({-8.f, 15.f, 8.f});
        mlg::Camera::GetInstance()->SetRotation(glm::radians(-60.f), glm::radians(45.f));

        auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json");
        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/plane.obj");

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, whiteMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -5.f, 0.f});
        ground.lock()->GetTransform().SetScale(glm::vec3{100.f});

        auto player = mlg::EntityManager::SpawnEntity<SimplePlayer>("Player", false, mlg::SceneGraph::GetRoot());
    }

    virtual ~ProjectInpostors() {
    }
};

void Test() {
}

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();
    std::srand(std::time(0));

    Test();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
