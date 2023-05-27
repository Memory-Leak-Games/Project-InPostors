#include "Core/SceneManager/SceneManager.h"
#include "Core/Core.h"

#include "Initializer.h"

#include "Scenes/LevelScene.h"
#include "Scenes/TestScene.h"

class ProjectInpostors {
public:
    ProjectInpostors() = default;

    int Main(int argc, char* argv[]) {
        mlg::Initializer::InitializeCoreComponents();
        mlg::Initializer::InitializeSceneComponents();

        mlg::Core* engine = mlg::Core::GetInstance();

        auto testScene = std::make_unique<TestScene>();
        mlg::SceneManager::LoadScene(std::move(testScene));
        engine->MainLoop();

        mlg::Initializer::StopSceneComponents();
        mlg::Initializer::StopCoreComponents();

        return 0;
    }

    virtual ~ProjectInpostors() {
    }
};

int main(int argc, char* argv[]) {
    LoggingMacros::InitializeSPDLog();

    ProjectInpostors game;
    return game.Main(argc, argv);
}
