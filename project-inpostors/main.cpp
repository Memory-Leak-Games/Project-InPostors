#include <ctime>

#include <CoreEngine.h>
#include <Nodes/FreeCameraNode.h>
#include <ShaderWrapper.h>
#include <Mesh/Model.h>
#include <Nodes/ModelNode.h>

class ProjectInpostors {
private:
public:
    ProjectInpostors() {
    }

    int Main(int argc, char* argv[]) {
        mlg::CoreEngine* engine = mlg::CoreEngine::GetInstance();

        PrepareScene();
        int32_t ReturnCode = engine->MainLoop();

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
    std::srand(std::time(0));
    ProjectInpostors game;
    return game.Main(argc, argv);
}
