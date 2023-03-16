// This is not allowed in Game layer
#include "GameplayLayer/Nodes/FreeCameraNode.h"
#include "GameplayLayer/Nodes/ModelNode.h"
#include "LowLevelRenderer/Model.h"
#include "LowLevelRenderer/ShaderWrapper.h"

#include "Core/CoreEngine.h"
#include "Core/Time.h"

class ProjectInpostors {
private:
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Time::Initialize();
        mlg::Window::InitWindow("Memory Leak Engine", 1280, 720);
        mlg::CoreEngine::Initialize();

        mlg::CoreEngine* engine = mlg::CoreEngine::GetInstance();

        PrepareScene();

        int32_t ReturnCode = engine->MainLoop();

        mlg::CoreEngine::Stop();
        mlg::Window::DestroyWindow();

        return ReturnCode;
    }

    void PrepareScene() {
        mlg::CoreEngine* engine = mlg::CoreEngine::GetInstance();

        auto camera = std::make_shared<mlg::FreeCameraNode>();
        engine->GetSceneRoot()->AddChild(camera);
        camera->GetLocalTransform()->SetPosition({0, 0, -20});
        camera->SetActive();

        auto modelShader = std::make_shared<mlg::ShaderWrapper>("res/shaders/instanced.vert", "res/shaders/textured_model.frag");

        auto tardisModel = std::make_shared<mlg::Model>("res/models/Tardis/tardis.obj", modelShader);
        auto tardisNode = std::make_shared<mlg::ModelNode>(tardisModel, engine->GetRenderer());
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
