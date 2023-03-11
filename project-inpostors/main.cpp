#include <ctime>

#include <CoreEngine.h>
#include <Nodes/FreeCameraNode.h>
#include <ShaderWrapper.h>
#include <Mesh/Model.h>
#include <Nodes/ModelNode.h>

class ProjectInpostors {
private:
    std::shared_ptr<mlg::CoreEngine> Engine;

public:
    ProjectInpostors() {
    }

    int Main(int argc, char* argv[]) {
        Engine = std::make_shared<mlg::CoreEngine>();

        if (Engine->Init() == 0) {
            PrepareScene();
            int32_t ReturnCode = Engine->MainLoop();

            if (ReturnCode != 0) {
                return ReturnCode;
            }
        }

        return 0;
    }

    void PrepareScene() {
        auto camera = std::make_shared<mlg::FreeCameraNode>(Engine.get());
        Engine->GetSceneRoot()->AddChild(camera);
        camera->GetLocalTransform()->SetPosition({0, 0, -20});
        camera->SetActive();

        auto modelShader = std::make_shared<mlg::ShaderWrapper>("res/shaders/instanced.vert", "res/shaders/textured_model.frag");

        auto tardisModel = std::make_shared<mlg::Model>("res/models/Tardis/tardis.obj", modelShader);
        auto tardisNode = std::make_shared<mlg::ModelNode>(tardisModel, Engine->GetRenderer());
        Engine->GetSceneRoot()->AddChild(tardisNode);
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    std::srand(std::time(0));
    ProjectInpostors game;
    return game.Main(argc, argv);
}
