// This is not allowed in Game layer
#include "Rendering/Camera.h"
#include "Rendering/Model.h"
#include "Rendering/Renderer.h"

#include <Rendering/RenderingAPI.h>

#include <Audio/AudioAPI.h>

#include <Rendering/Assets/MaterialAsset.h>
#include <Rendering/Assets/ModelAsset.h>

#include <Core/HID/Input.h>
#include <Core/AssetManager/AssetManager.h>

#include "Core/Core.h"
#include "Core/Time.h"

#include "SceneGraph/SceneGraph.h"

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
        mlg::AudioAPI::Initialize();
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
        mlg::AudioAPI::Stop();
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

        auto tardisMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Tardis/tardis_material.json");
        auto tardisModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Tardis/tardis.obj");

        auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json");
        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/plane.obj");

        auto carModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Cars/autko1.obj");
        auto carEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("CarOne", false, mlg::SceneGraph::GetRoot());
        carEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", carModel, whiteMaterial);
        carEntity.lock()->AddComponent<ComponentTest>("RotationComponent");

        auto buildingModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Buildings/dom1.obj");

        auto buildingEntityOne = mlg::EntityManager::SpawnEntity<mlg::Entity>("TardisRight", false, mlg::SceneGraph::GetRoot());
        buildingEntityOne.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", buildingModel, whiteMaterial);
        buildingEntityOne.lock()->AddComponent<ComponentTest>("RotationComponent");

        buildingEntityOne.lock()->GetTransform().SetPosition({-7.f, 0.f, 0.f});
        buildingEntityOne.lock()->GetTransform().SetScale(glm::vec3{2.});

        auto tardisMaterial2 = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Tardis/tardis_material_another.json");
        auto tardisModel2 = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Tardis/tardis.obj");

        auto tardisEntity2 = mlg::EntityManager::SpawnEntity<mlg::Entity>("TardisLeft", false, mlg::SceneGraph::GetRoot());
        tardisEntity2.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", tardisModel2, tardisMaterial2);
        auto rigidbody = tardisEntity2.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        rigidbody.lock()->AddForce({0, 40.f});
        rigidbody.lock()->AddForce({-40.f, 0.f}, {0.f, 1.f});

        rigidbody.lock()->SetLinearDrag(1.f);
        rigidbody.lock()->SetAngularDrag(1.f);

        tardisEntity2.lock()->GetTransform().SetPosition({7.f, 0.f, 0.f});

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, whiteMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -5.f, 0.f});
        ground.lock()->GetTransform().SetScale(glm::vec3{100.f});

        auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/comic.ttf");

        label = std::make_shared<mlg::Label>();
        label->font = font;

        auto imageMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/cat_UI_material.json");
        image = std::make_shared<mlg::Image>(imageMaterial);
        image->SetSize(glm::vec2{256.f});
        image->SetPosition({50.f, 50.f});

        auto progressBarMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/progressBar_material.json");

        progressBar = std::make_shared<mlg::ProgressBar>(progressBarMaterial);
        progressBar->SetSize(glm::vec2{256.f, 32.f});
        progressBar->SetPosition({50.f, 400.f});

        mlg::Renderer2D::GetInstance()->AddRenderable(label);
        mlg::Renderer2D::GetInstance()->AddRenderable(image);
        mlg::Renderer2D::GetInstance()->AddRenderable(progressBar);
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
