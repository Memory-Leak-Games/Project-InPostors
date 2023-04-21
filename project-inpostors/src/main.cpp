// This is not allowed in Game layer
#include "Rendering/Model.h"
#include "Rendering/Renderer.h"

#include <Rendering/RenderingAPI.h>

#include <Rendering/Assets/MaterialAsset.h>
#include <Rendering/Assets/ModelAsset.h>

#include <Core/AssetManager/AssetManager.h>
#include <Core/HID/Input.h>

#include "Core/Core.h"
#include "Core/Time.h"

#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/Levels/LevelGenerator.h"
#include "Player.h"
#include "SceneGraph/SceneGraph.h"

#include <Core/Math.h>
#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/RigidbodyComponent.h>
#include <Gameplay/Components/StaticMeshComponent.h>
#include <Gameplay/EntityManager.h>
#include <Physics/Physics.h>
#include <Rendering/CommonUniformBuffer.h>
#include <Rendering/Gizmos/Gizmos.h>

#include <UI/Assets/FontAsset.h>
#include <UI/Components/Image.h>
#include <UI/Components/Label.h>
#include <UI/Components/ProgressBar.h>
#include <UI/Renderer2D.h>

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
        mlg::SettingsManager::Initialize();

        mlg::Time::Initialize();
        mlg::AssetManager::Initialize();
        mlg::Window::Initialize("Memory Leak Engine");
        mlg::RenderingAPI::Initialize();
        mlg::Renderer::Initialize();
        mlg::Renderer2D::Initialize();
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
        mlg::Renderer2D::Stop();
        mlg::Renderer::Stop();
        mlg::RenderingAPI::Stop();
        mlg::Window::Stop();
        mlg::AssetManager::Stop();
        mlg::Time::Stop();

        mlg::SettingsManager::Initialize();

        return 0;
    }

    void PrepareScene() {
        auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
        auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");
        //        cameraComponent.lock()->SetPerspective(glm::radians(90.f), 0.1, 100.f);
        cameraComponent.lock()->SetOrtho(40.f, 0.1, 100.f);

        cameraComponent.lock()->GetTransform().SetPosition({-10.f, 15.f, -10.f});
        cameraComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{60.f, 45.f, 0.f}));

        auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json");
        auto redMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/red_material.json");
        auto blueMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/blue_material.json");

        auto groundMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Ground/ground_material.json");
        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/plane.obj");

        auto cubeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Cube.obj");
        auto sphereModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Sphere.obj");

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, groundMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -0.5f, 0.f});
        ground.lock()->GetTransform().SetScale(glm::vec3{100.f});

        auto ui = mlg::EntityManager::SpawnEntity<mlg::Entity>("ui", true, mlg::SceneGraph::GetRoot());

        auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/comic.ttf");
        auto label = ui.lock()->AddComponent<mlg::Label>("Label", font);
        label.lock()->SetPosition({10, 10});

        auto imageMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/cat_UI_material.json");
        auto image = ui.lock()->AddComponent<mlg::Image>("Image", imageMaterial);
        image.lock()->SetSize(glm::vec2{256.f});
        image.lock()->SetPosition({50.f, 50.f});

        auto progressBarMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/progressBar_material.json");
        auto progressBar = ui.lock()->AddComponent<mlg::ProgressBar>("ProgressBar", progressBarMaterial);
        progressBar.lock()->SetSize(glm::vec2{256.f, 32.f});
        progressBar.lock()->SetPosition({50.f, 400.f});

        mlg::Renderer2D::GetInstance()->AddRenderable(label);
        mlg::Renderer2D::GetInstance()->AddRenderable(image);
        mlg::Renderer2D::GetInstance()->AddRenderable(progressBar);

        auto player = mlg::EntityManager::SpawnEntity<Player>("Player", false, mlg::SceneGraph::GetRoot());

        SpawnSpheres();

        //        mlg::LevelGenerator::LoadJson("res/levels/detroit.json");
        //        mlg::LevelGenerator::GenerateLevel(7.0f);
    }

    void SpawnSpheres() {
        const int size = 350;
        const int sizeX = std::floor(std::sqrt(size));
        const int sizeY = size / sizeX;

        const float distance = 3.f;

        auto sphereModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Sphere.obj");
        auto blueMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/blue_material.json");

        for (int i = 0; i < sizeX; ++i) {
            for (int j = 0; j < sizeY; ++j) {
                glm::vec2 offset = {sizeX * distance / 2.f, sizeY * distance / 2.f};

                auto sphere = mlg::EntityManager::SpawnEntity<mlg::Entity>("Sphere", false, mlg::SceneGraph::GetRoot());
                sphere.lock()->GetTransform().SetPosition({(float) i * distance - offset.x, 0.f, (float) j * distance - offset.y});

                auto sphereMesh = sphere.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", sphereModel, blueMaterial);
                sphereMesh.lock()->GetTransform().SetScale(glm::vec3{2.f});

                auto sphereRigidbody = sphere.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
                sphereRigidbody.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f), 1.f);
                sphereRigidbody.lock()->SetLinearDrag(2.f);
                sphereRigidbody.lock()->SetAngularDrag(2.f);
            }
        }

        SPDLOG_WARN("Number of colliders: {}", sizeX * sizeY);
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
