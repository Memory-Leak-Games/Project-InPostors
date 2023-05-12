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

#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/RigidbodyComponent.h>
#include <Gameplay/Components/StaticMeshComponent.h>
#include <Gameplay/EntityManager.h>
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

        mlg::Core::Initialize();
        mlg::Input::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

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

        auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/ARLRDBD.TTF");
        auto label = ui.lock()->AddComponent<mlg::Label>("Label", font);
        label.lock()->SetText("Aaa Kotki 2 szarobure obydwa 1234567890");
        label.lock()->SetPosition({50, 50});
        label.lock()->SetTextColor({1, 1, 1});
        label.lock()->SetSize(32);

        auto imageMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/cat_UI_material.json");
        auto image = ui.lock()->AddComponent<mlg::Image>("Image", imageMaterial);
        image.lock()->SetSize(glm::vec2{256.f});
        image.lock()->SetPosition({1280.f - 128.f, 720.f - 128.f});
        image.lock()->SetAnchor({1, 1});

        auto progressBarMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/UI/progressBar_material.json");
        auto progressBar = ui.lock()->AddComponent<mlg::ProgressBar>("ProgressBar", progressBarMaterial);
        progressBar.lock()->SetSize(glm::vec2{256.f, 32.f});
        progressBar.lock()->SetPosition({150.f, 600.f});
        progressBar.lock()->SetAnchor({0, 1});

        mlg::Renderer2D::GetInstance()->AddRenderable(label);
        mlg::Renderer2D::GetInstance()->AddRenderable(image);
        mlg::Renderer2D::GetInstance()->AddRenderable(progressBar);

        PlayerData firstPlayerData = {0, mlg::RGBA::red};
        PlayerData secondPlayerData = {0, mlg::RGBA::cyan};

        auto player = mlg::EntityManager::SpawnEntity<Player>("Player", false, mlg::SceneGraph::GetRoot(), firstPlayerData);
        player.lock()->AddComponent<PlayerOneInput>("PlayerInput");

        auto playerTwo = mlg::EntityManager::SpawnEntity<Player>("PlayerTwo", false, mlg::SceneGraph::GetRoot(), secondPlayerData);
        playerTwo.lock()->AddComponent<PlayerTwoInput>("PlayerInput");

        auto cameraEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("Camera", false, mlg::SceneGraph::GetRoot());
        auto cameraComponent = cameraEntity.lock()->AddComponent<mlg::CameraComponent>("CameraComponent");

        mlg::LevelGenerator::LoadMap("res/levels/Cities/detroit.json");
        mlg::LevelGenerator::SpawnGround("res/levels/Cities/detroit.json");
        mlg::LevelGenerator::LoadCameraSettings("res/levels/Cities/detroit.json", *cameraComponent.lock());
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
