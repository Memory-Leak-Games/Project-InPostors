// This is not allowed in Game layer
#include "GameplayLayer/Nodes/FreeCameraNode.h"
#include "GameplayLayer/Nodes/ModelNode.h"
#include "LowLevelRenderer/Model.h"
#include "LowLevelRenderer/ShaderWrapper.h"
#include <Core/HID/Input.h>
#include <Core/AssetManager/AssetManager.h>
#include <LowLevelRenderer/RenderingAPI.h>

#include "Core/Core.h"
#include "Core/Time.h"

class ProjectInpostors {
private:
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Time::Initialize();
        mlg::Window::Initialize("Memory Leak Engine", 1280, 720);
        mlg::Window::GetInstance()->SetVerticalSync(false);
        mlg::RenderingAPI::Initialize();

        mlg::AssetManager::Initialize();
        mlg::Core::Initialize();
        mlg::Input::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        int32_t returnCode = engine->MainLoop();

        mlg::Core::Stop();
        mlg::Window::Stop();
        mlg::Input::Stop();
        mlg::RenderingAPI::Stop();
        mlg::AssetManager::Stop();

        return returnCode;
    }

    void PrepareScene() {
        mlg::Core* engine = mlg::Core::GetInstance();

        auto camera = std::make_shared<mlg::FreeCameraNode>();
        engine->GetSceneRoot()->AddChild(camera);
        camera->GetLocalTransform()->SetPosition({0, 0, -20});
        camera->SetActive();

        auto modelShader = std::make_shared<mlg::ShaderWrapper>("res/shaders/model.vert", "res/shaders/textured_model.frag");

        auto tardisModel = std::make_shared<mlg::Model>("res/models/Tardis/tardis.obj", modelShader);
        auto tardisNode = std::make_shared<mlg::ModelNode>(tardisModel);
        engine->GetSceneRoot()->AddChild(tardisNode);
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
