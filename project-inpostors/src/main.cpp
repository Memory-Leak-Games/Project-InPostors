// This is not allowed in Game layer
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
#include "Player.h"
#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/Levels/LevelGenerator.h"

#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/StaticMeshComponent.h>
#include <Gameplay/EntityManager.h>
#include <Rendering/CommonUniformBuffer.h>
#include <Rendering/Gizmos/Gizmos.h>
#include <Physics/Physics.h>
#include <Core/Math.h>
#include <Gameplay/Components/RigidbodyComponent.h>

#include <UI/Assets/FontAsset.h>
#include <UI/Components/Image.h>
#include <UI/Components/Label.h>
#include <UI/Components/ProgressBar.h>
#include <UI/Renderer2D.h>

class ComponentTest : public mlg::Component {
public:
    ComponentTest(const std::weak_ptr<mlg::Entity> &owner, const std::string &name) : Component(owner, name) {}

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

    int Main(int argc, char *argv[]) {
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

        mlg::Core *engine = mlg::Core::GetInstance();
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

        auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/ARLRDBD.ttf");
        auto label = ui.lock()->AddComponent<mlg::Label>("Label", font);
        label.lock()->SetText("Aaa Kotki 2 szarobure obydwa 1234567890");
        label.lock()->SetPosition({50, 50});
        label.lock()->SetTextColor({1, 1, 1});
        label.lock()->SetSize(32);

        auto imageMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/cat_UI_material.json");
        auto image = ui.lock()->AddComponent<mlg::Image>("Image", imageMaterial);
        image.lock()->SetSize(glm::vec2{256.f});
        image.lock()->SetPosition({1280.f-128.f, 720.f-128.f});
        image.lock()->SetAnchor({1, 1});

        auto progressBarMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                "res/materials/UI/progressBar_material.json");
        auto progressBar = ui.lock()->AddComponent<mlg::ProgressBar>("ProgressBar", progressBarMaterial);
        progressBar.lock()->SetSize(glm::vec2{256.f, 32.f});
        progressBar.lock()->SetPosition({150.f, 600.f});
        progressBar.lock()->SetAnchor({0, 1});

        mlg::Renderer2D::GetInstance()->AddRenderable(label);
        mlg::Renderer2D::GetInstance()->AddRenderable(image);
        mlg::Renderer2D::GetInstance()->AddRenderable(progressBar);

        auto wall = mlg::EntityManager::SpawnEntity<mlg::Entity>("Wall", true, mlg::SceneGraph::GetRoot());
        wall.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", cubeModel, redMaterial);
        wall.lock()->GetTransform().SetPosition({-2.f, 0.f, -5.f});
        auto rigidbody = wall.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        rigidbody.lock()->AddCollider<mlg::ColliderShape::Rectangle>(glm::vec2(0.f), glm::vec2(1.f));

        auto box = mlg::EntityManager::SpawnEntity<mlg::Entity>("Box", false, mlg::SceneGraph::GetRoot());
        box.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", cubeModel, blueMaterial);
        box.lock()->GetTransform().SetPosition({2.f, 0.f, -5.f});
        auto boxRigidbody = box.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        boxRigidbody.lock()->AddCollider<mlg::ColliderShape::Rectangle>(glm::vec2(0.f), glm::vec2(1.f));
        boxRigidbody.lock()->SetLinearDrag(2.f);
        boxRigidbody.lock()->SetAngularDrag(2.f);

        auto sphere = mlg::EntityManager::SpawnEntity<mlg::Entity>("Sphere", false, mlg::SceneGraph::GetRoot());
        auto sphereMesh = sphere.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", sphereModel,
                                                                                blueMaterial);
        sphereMesh.lock()->GetTransform().SetScale(glm::vec3{2.f});
        sphere.lock()->GetTransform().SetPosition({2.f, 0.f, 5.f});
        auto sphereRigidbody = sphere.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        sphereRigidbody.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f), 1.f);
        sphereRigidbody.lock()->SetLinearDrag(2.f);
        sphereRigidbody.lock()->SetAngularDrag(2.f);

        auto player = mlg::EntityManager::SpawnEntity<Player>("Player", false, mlg::SceneGraph::GetRoot());

//        SpawnHouses();
        mlg::LevelGenerator::LoadJson("res/levels/detroit.json");
        mlg::LevelGenerator::GenerateLevel(7.0f);
    }

    void SpawnHouses() {
        using Random = effolkronium::random_static;

        std::vector<std::shared_ptr<mlg::ModelAsset>> models;
        models.push_back(mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Buildings/House_1.obj"));
        models.push_back(mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Buildings/House_2.obj"));
        models.push_back(mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Buildings/House_3.obj"));

        float size = 10.f;
        int city_size = 3;
        float city_size_in_units = (float) city_size * size;

        for (int i = 0; i < city_size; ++i) {
            for (int j = 0; j < city_size; ++j) {
                if (i % 3)
                    Random::shuffle(models);

                auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                        "res/models/Primitives/white_material.json");
                auto model = models[i % 3];

                glm::vec3 buildingPosition{0.f};
                buildingPosition.x = (float) i * size - city_size_in_units / 2.f;
                buildingPosition.y = -0.5;
                buildingPosition.z = (float) j * size - city_size_in_units / 2.f;

                auto building = mlg::EntityManager::SpawnEntity<mlg::Entity>("Wall", true, mlg::SceneGraph::GetRoot());
                building.lock()->GetTransform().SetPosition(buildingPosition);
                building.lock()->GetTransform().SetScale(glm::vec3(2.f));

                building.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", model, whiteMaterial);
                auto rigidbody = building.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
                rigidbody.lock()->AddCollider<mlg::ColliderShape::Rectangle>(glm::vec2(0.f), glm::vec2(4.f));
                rigidbody.lock()->SetBounciness(0.2f);
            }
        }
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char *argv[]) {
    LoggingMacros::InitializeSPDLog();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
