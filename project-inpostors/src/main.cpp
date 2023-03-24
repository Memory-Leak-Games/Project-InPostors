// This is not allowed in Game layer
#include "Rendering/Camera.h"
#include "Rendering/Model.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderWrapper.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/HID/Input.h>
#include <Rendering/Gizmos/Gizmos.h>
#include <Rendering/Renderable.h>
#include <Rendering/RenderingAPI.h>

#include "Core/Core.h"
#include "Core/Time.h"
#include "SceneGraph/Transform.h"

class RenderableTest : public mlg::Renderable {
public:
    mlg::Model model;
    mlg::Transform transform;

    RenderableTest(std::string modelPath, std::string vert, std::string frag)
    : model(modelPath, std::make_shared<mlg::ShaderWrapper>(vert, frag)),
    transform() {}

    void Draw(mlg::Renderer* renderer) override {
        model.GetShader()->Activate();
        model.GetShader()->SetMat4F("World", transform.GetLocalMatrix());
        model.Draw();
    }

};

class ProjectInpostors {
private:
    std::shared_ptr<mlg::Camera> camera;
    std::shared_ptr<RenderableTest> tardis;
    std::shared_ptr<RenderableTest> tardis2;
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Time::Initialize();
        mlg::Window::Initialize("Memory Leak Engine", 1280, 720);
        mlg::Window::GetInstance()->SetVerticalSync(false);
        mlg::RenderingAPI::Initialize();
        mlg::Renderer::Initialize();
        mlg::Gizmos::Initialize();
        mlg::AssetManager::Initialize();

        mlg::Core::Initialize();
        mlg::Input::Initialize();

        mlg::Core* engine = mlg::Core::GetInstance();
        PrepareScene();
        engine->MainLoop();

        mlg::Input::Stop();
        mlg::Core::Stop();
        mlg::AssetManager::Stop();
        //mlg::Gizmos::Stop();
        mlg::Renderer::Stop();
        mlg::RenderingAPI::Stop();
        mlg::Window::Stop();
        mlg::Time::Stop();

        return 0;
    }

    void PrepareScene() {
        mlg::Camera::GetInstance()->SetPosition({0, 0, -20});
        tardis = std::make_shared<RenderableTest>("res/models/Tardis/tardis.obj", "res/shaders/model.vert", "res/shaders/textured_model.frag");
        tardis->transform.SetPosition({-5, 0, 0});
        mlg::Renderer::GetInstance()->AddRenderable(tardis);

        tardis2 = std::make_shared<RenderableTest>("res/models/Tardis/tardis.obj", "res/shaders/model.vert", "res/shaders/textured_model.frag");
        tardis2->transform.SetPosition({5, 0, 0});
        mlg::Renderer::GetInstance()->AddRenderable(tardis2);
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
