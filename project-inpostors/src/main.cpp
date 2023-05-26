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

#include "Buildings/Factory.h"
#include "Car/PlayerOneInput.h"
#include "Car/PlayerTwoInput.h"
#include "Core/TimerManager.h"
#include "Core/Settings/SettingsManager.h"

#include "Gameplay/Components/CameraComponent.h"
#include "Levels/LevelGenerator.h"
#include "Player.h"
#include "SceneGraph/SceneGraph.h"
#include "Car/PlayerOneInput.h"
#include "Car/PlayerTwoInput.h"
#include "Buildings/Factory.h"
#include "ai/TrafficCar.h"
#include "ai/AIComponent.h"
#include "ai/SteeringBehaviors.h"
#include "ai/Path.h"

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
#include <UI/Components/Button.h>
#include <UI/UIRenderer.h>

#include <Physics/Colliders/Collider.h>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <UI/UIController.h>
#include <string>

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
        mlg::TimerManager::Initialize();
        mlg::AssetManager::Initialize();
        mlg::Window::Initialize("Memory Leak Engine");
        mlg::RenderingAPI::Initialize();
        mlg::Renderer::Initialize();
        mlg::UIRenderer::Initialize();
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
        mlg::UIController::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

        mlg::AI::Stop();

        mlg::EntityManager::Stop();
        mlg::ComponentManager::Stop();

        mlg::Physics::Stop();

        mlg::AudioAPI::Stop();
        mlg::SceneGraph::Stop();
        mlg::UIController::Stop();
        mlg::Input::Stop();
        mlg::Core::Stop();
        mlg::Gizmos::Stop();
        mlg::UIRenderer::Stop();
        mlg::Renderer::Stop();
        mlg::RenderingAPI::Stop();
        mlg::Window::Stop();
        mlg::AssetManager::Stop();
        mlg::Time::Stop();
        mlg::TimerManager::Stop();

        mlg::SettingsManager::Stop();

        return 0;
    }

    void PrepareScene() {
        auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
        auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

        std::vector<std::string> levelLayout;

        const std::string testLevelPath = "res/levels/maps/detroit.json";
        const std::string testLevelPropsPath = "res/levels/maps/detroit_props.json";

        levelLayout = mlg::LevelGenerator::LoadMap(testLevelPath);
        mlg::LevelGenerator::LoadMap(testLevelPath);
        mlg::LevelGenerator::SpawnGround(testLevelPath);
        mlg::LevelGenerator::SetCityBounds(testLevelPath);
        mlg::LevelGenerator::LoadCameraSettings(testLevelPath, *cameraComponent.lock());
        mlg::LevelGenerator::SpawnPlayers(testLevelPath);

        // load props
        mlg::LevelGenerator::LoadMap(testLevelPropsPath);

        TrafficCarData testCarOneData = {0, mlg::RGBA::white};
        TrafficCarData testCarTwoData = {1, mlg::RGBA::gray};
        TrafficCarData testCarThreeData = {2, mlg::RGBA::black};

        auto testCarOne = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar", false, mlg::SceneGraph::GetRoot(), testCarOneData);
        testCarOne.lock()->GetComponentByName<AIComponent>("AIMovementComponent").lock()->GetSteering()->TrafficDriveOn();
        auto testCarOneRigidbody = testCarOne.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");
        testCarOneRigidbody.lock()->SetPosition({10.f, 15.f});

//        auto testCarTwo = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar", false, mlg::SceneGraph::GetRoot(), testCarTwoData);
//        testCarTwo.lock()->GetComponentByName<AIComponent>("AIMovementComponent").lock()->GetSteering()->TrafficDriveOn();
//        auto testCarTwoRigidbody = testCarTwo.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");
//        testCarTwoRigidbody.lock()->SetPosition({10.f, 25.f});
//
//        auto testCarThree = mlg::EntityManager::SpawnEntity<TrafficCar>("TrafficCar", false, mlg::SceneGraph::GetRoot(), testCarThreeData);
//        testCarThree.lock()->GetComponentByName<AIComponent>("AIMovementComponent").lock()->GetSteering()->TrafficDriveOn();
//        auto testCarThreeRigidbody = testCarThree.lock()->GetComponentByName<mlg::RigidbodyComponent>("Rigidbody");
//        testCarThreeRigidbody.lock()->SetPosition({-30.f, 0.f});

        // create factories
        auto testFactory = mlg::EntityManager::SpawnEntity<Factory>("Smelter", false, mlg::SceneGraph::GetRoot(),
                                                                  "res/levels/factories/smelter.json");
        auto testFactoryRigidBody = testFactory.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testFactoryRigidBody.lock()->SetPosition({22.f, 8.f});

        auto testMine = mlg::EntityManager::SpawnEntity<Factory>("Mine", false, mlg::SceneGraph::GetRoot(),
                                                                    "res/levels/factories/mine.json");
        auto testMineRigidBody = testMine.lock()->GetComponentByName<mlg::RigidbodyComponent>("MainRigidbody");
        testMineRigidBody.lock()->SetPosition({-60.f, -5.f});
        testMineRigidBody.lock()->SetRotation(glm::radians(-90.f));

        auto testIkea = mlg::EntityManager::SpawnEntity<Factory>("Szwedzki sklep z meblami", false, mlg::SceneGraph::GetRoot(),
                                                                 "res/levels/factories/ikea.json");
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
