#include "Rendering/Model.h"
#include "Rendering/Renderer.h"

#include <Rendering/RenderingAPI.h>

#include <Audio/Assets/AudioAsset.h>
#include <Audio/AudioAPI.h>

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
#include "Car/PlayerOneInput.h"
#include "Car/PlayerTwoInput.h"
#include "Buildings/Factory.h"
#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"
#include "ai/SteeringBehaviors.h"

#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/RigidbodyComponent.h>
#include <Gameplay/Components/StaticMeshComponent.h>
#include <Gameplay/EntityManager.h>
#include <Gameplay/AI/AI.h>
#include <Physics/Physics.h>
#include <Rendering/CommonUniformBuffer.h>
#include <Rendering/Gizmos/Gizmos.h>

#include <Gameplay/Components/AudioComponent.h>

#include <UI/Assets/FontAsset.h>
#include <UI/Components/Image.h>
#include <UI/Components/Label.h>
#include <UI/Components/ProgressBar.h>
#include <UI/Renderer2D.h>

#include <Physics/Colliders/Collider.h>

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
    std::shared_ptr<mlg::AudioAsset> sound;
    std::shared_ptr<mlg::AudioAsset> music;

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
        mlg::AudioAPI::Initialize();

        mlg::AudioAPI::GetSoLoud()->setGlobalVolume(mlg::SettingsManager::Get<float>(mlg::SettingsType::Audio, "volume"));

        mlg::Physics::Initialize();

        mlg::ComponentManager::Initialize();
        mlg::EntityManager::Initialize();

        mlg::AI::Initialize();

        mlg::Core::Initialize();
        mlg::Input::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

        mlg::AI::Stop();

        mlg::EntityManager::Stop();
        mlg::ComponentManager::Stop();

        mlg::Physics::Stop();

        mlg::AudioAPI::Stop();
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

        mlg::SettingsManager::Stop();

        return 0;
    }

    void PrepareScene() {
//        sound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/SFX/mario_coin.ogg");
//        music = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/music/Crushin.ogg");
//        auto audioComponent = cameraEntity.lock()->AddComponent<mlg::AudioComponent>("AudioComponent", music);
//        audioComponent.lock()->SetLooping();
//        audioComponent.lock()->Play();

        auto whiteMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/white_material.json");
        auto redMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/red_material.json");
        auto blueMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/blue_material.json");

        auto cubeModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Cube.obj");
        auto sphereModel = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Sphere.obj");

        auto ui = mlg::EntityManager::SpawnEntity<mlg::Entity>("ui", true, mlg::SceneGraph::GetRoot());

        auto imageMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/cat_UI_material.json");
        auto image = ui.lock()->AddComponent<mlg::Image>("Image", imageMaterial);
        image.lock()->SetSize(glm::vec2{128.f});
        image.lock()->SetPosition({1280.f - 128.f, 720.f - 128.f});
        image.lock()->SetAnchor({1, 1});

        mlg::Renderer2D::GetInstance()->AddRenderable(image);

        PlayerData firstPlayerData = {0, mlg::RGBA::red};
        PlayerData secondPlayerData = {0, mlg::RGBA::cyan};

        auto player = mlg::EntityManager::SpawnEntity<Player>("Player", false, mlg::SceneGraph::GetRoot(), firstPlayerData);
        player.lock()->AddComponent<PlayerOneInput>("PlayerInput");

        auto playerTwo = mlg::EntityManager::SpawnEntity<Player>("PlayerTwo", false, mlg::SceneGraph::GetRoot(), secondPlayerData);
        playerTwo.lock()->AddComponent<PlayerTwoInput>("PlayerInput");

        TrafficCarData testCarData = {0, mlg::RGBA::white};

        auto testCar = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar", false, mlg::SceneGraph::GetRoot(), testCarData);
        testCar.lock()->GetComponentByName<AIComponent>("AIMovementComponent").lock()->GetSteering()->SeekOn();

        auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
        auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

        //std::vector<std::string> levelLayout;
        //levelLayout = mlg::LevelGenerator::LoadMap("res/levels/Cities/detroit.json");
        mlg::LevelGenerator::LoadMap("res/levels/Cities/detroit.json");
        mlg::LevelGenerator::SpawnGround("res/levels/Cities/detroit.json");
        mlg::LevelGenerator::SetCityBounds("res/levels/Cities/detroit.json");
        mlg::LevelGenerator::LoadCameraSettings("res/levels/Cities/detroit.json", *cameraComponent.lock());

        // load props
        mlg::LevelGenerator::LoadMap("res/levels/Cities/detroit_props.json");

        // create factories
        auto testFactory = mlg::EntityManager::SpawnEntity<Factory>("TestFactory", false, mlg::SceneGraph::GetRoot(),
                                                                    "res/levels/Factories/smelter.json");
        auto testFactoryRigidBody = testFactory.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testFactoryRigidBody.lock()->SetPosition({22.f, 8.f});

        auto testMine = mlg::EntityManager::SpawnEntity<Factory>("TestFactory", false, mlg::SceneGraph::GetRoot(),
                                                                    "res/levels/Factories/mine.json");
        auto testMineRigidBody = testMine.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testMineRigidBody.lock()->SetPosition({-60.f, -5.f});
        testMineRigidBody.lock()->SetRotation(glm::radians(-90.f));

        auto testIkea = mlg::EntityManager::SpawnEntity<Factory>("TestFactory", false, mlg::SceneGraph::GetRoot(),
                                                                 "res/levels/Factories/ikea.json");
        auto testIkeaRigidBody = testIkea.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testIkeaRigidBody.lock()->SetPosition({55.f, -5.f});
        testIkeaRigidBody.lock()->SetRotation(glm::radians(-90.f));
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
