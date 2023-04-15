// This is not allowed in Game layer
#include "Rendering/Camera.h"
#include "Rendering/Model.h"
#include "Rendering/Renderer.h"
#include "UI/Renderer2D.h"

#include <Rendering/RenderingAPI.h>

#include <Rendering/Assets/MaterialAsset.h>
#include <Rendering/Assets/ModelAsset.h>

#include <Core/HID/Input.h>
#include <Core/AssetManager/AssetManager.h>

#include "Core/Core.h"
#include "Core/Time.h"

#include "SceneGraph/SceneGraph.h"
#include "SimplePlayer.h"
#include "Core/Settings/SettingsManager.h"

#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/StaticMeshComponent.h>
#include <Gameplay/EntityManager.h>
#include <Rendering/CommonUniformBuffer.h>
#include <Rendering/Gizmos/Gizmos.h>
#include <Gameplay/Levels/LevelGenerator.h>

#include "soloud.h"
#include "soloud_wav.h"
#include "Physics/Physics.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Label.h"
#include "UI/Components/Image.h"
#include "UI/Components/ProgressBar.h"
#include "Gameplay/Components/RigidbodyComponent.h"

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

        mlg::SettingsManager::Initialize();

        return 0;
    }

    void PrepareScene() {
        mlg::LevelGenerator::GenerateTestLevel();
        mlg::Camera::GetInstance()->SetPosition({-8.f, 15.f, 8.f});
        mlg::Camera::GetInstance()->SetRotation(glm::radians(-60.f), glm::radians(45.f));
//        mlg::Camera::GetInstance()->SetRotation(glm::radians(-90.f), glm::radians(0.f));

        auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json");
        auto redMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/red_material.json");
        auto blueMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/blue_material.json");

        auto planeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/plane.obj");
        auto cubeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Cube.obj");
        auto sphereModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Sphere.obj");

        auto ground = mlg::EntityManager::SpawnEntity<mlg::Entity>("Ground", true, mlg::SceneGraph::GetRoot());
        ground.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", planeModel, whiteMaterial);
        ground.lock()->GetTransform().SetPosition({0.f, -1.f, 0.f});
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

        auto sphere = mlg::EntityManager::SpawnEntity<mlg::Entity>("Sphere", false, mlg::SceneGraph::GetRoot());
        sphere.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", sphereModel, blueMaterial);
        sphere.lock()->GetTransform().SetPosition({2.f, 0.f, 5.f});
        auto sphereRigidbody = sphere.lock()->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
        sphereRigidbody.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f), 1.f);

        auto player = mlg::EntityManager::SpawnEntity<SimplePlayer>("Player", false, mlg::SceneGraph::GetRoot());

        // DELETE ME
        SoLoud::Soloud gSoloud; // SoLoud engine
        SoLoud::Wav gWave;
        auto test = gSoloud.init();
        auto test2 = gWave.load("res/sound/test.wav");
        gSoloud.play(gWave); // Play the wave
        mlg::Time::Sleep(2.f);
        gSoloud.deinit();
    }

    virtual ~ProjectInpostors() {
    }
};

void Test() {
    mlg::LevelGenerator::LoadJson("res/levels/detroit.json");
}

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();
    std::srand(std::time(0));

    Test();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
