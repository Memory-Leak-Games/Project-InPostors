// This is not allowed in Game layer
#include "Rendering/Camera.h"
#include "Rendering/Model.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/Renderer.h"
#include <Rendering/RenderingAPI.h>
#include <Rendering/Renderable.h>

#include <Core/HID/Input.h>
#include <Core/AssetManager/AssetManager.h>
#include "Core/Core.h"
#include "Core/Time.h"

#include "SceneGraph/Transform.h"
#include "SceneGraph/SceneGraph.h"

#include <Gameplay/EntityManager.h>
#include <Gameplay/ComponentManager.h>
#include <Gameplay/Components/StaticMeshComponent.h>

class ComponentTest: public mlg::Component {
public:
    ComponentTest(const std::weak_ptr<mlg::Entity>& owner, const std::string& name) : Component(owner, name) {}

    void Update() override {
        GetOwner().lock()->GetTransform().SetRotation({{0.f, mlg::Time::GetSeconds(), 0.f}});

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
        auto tardisShader = std::make_shared<mlg::ShaderProgram>("res/shaders/model.vert", "res/shaders/textured_model.frag");
        auto tardisModel = std::make_shared<mlg::Model>("res/models/Tardis/tardis.obj", tardisShader);

        auto tardisEntity = mlg::EntityManager::SpawnEntity<mlg::Entity>("TardisOne", true, mlg::SceneGraph::GetRoot());
        tardisEntity.lock()->AddComponent<mlg::StaticMeshComponent>("StaticMesh", tardisModel);
        tardisEntity.lock()->AddComponent<ComponentTest>("RotationComponent");
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
