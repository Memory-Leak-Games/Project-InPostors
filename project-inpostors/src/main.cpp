#include <iostream>
#include <cxxopts.hpp>

#include "Core/SceneManager/SceneManager.h"
#include "Core/Core.h"

#include "Core/Window.h"
#include "Initializer.h"

#include "Scenes/LevelScene.h"
#include "Scenes/MenuScene.h"
#include "Scenes/TestScene.h"

class ProjectInpostors {
public:
    ProjectInpostors() = default;
    std::unique_ptr<mlg::Scene> startupScene;
    bool fullscreen_override = false;

    bool ParseArguments(int argc, char* argv[]) {
        cxxopts::Options options("Project Inpostors", "A game about couriers");
        options.allow_unrecognised_options();
        options.add_options()
            ("h,help", "Show this help")
            ("test-scene", "Run test scene")
            ("f,fullscreen", "Run in fullscreen mode")
            ("l,level", "provide level path", cxxopts::value<std::string>());

        auto result = options.parse(argc, argv);

        if (result.count("fullscreen")) {
            fullscreen_override = true;
        }

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return true;
        }

        if (result.count("test-scene")) {
            startupScene = std::move(std::make_unique<TestScene>());

            return false;
        }

        if (result.count("level")) {
            std::string path = result["level"].as<std::string>();

            auto levelScene = std::make_unique<LevelScene>(path);
            startupScene = std::move(levelScene);

            return false;
        }

        startupScene = std::move(std::make_unique<MenuScene>());
        return false;
    }

    int Main() {
        mlg::Initializer::InitializeCoreComponents();
        mlg::Initializer::InitializeSceneComponents();

        mlg::Core* engine = mlg::Core::Get();

        mlg::SceneManager::SetNextScene(std::move(startupScene));

        if (fullscreen_override) {
            mlg::Window::Get()->SetWindowType(mlg::WindowType::Fullscreen);
        }

        while (mlg::SceneManager::HasNextScene()) {
            mlg::SceneManager::LoadNextScene();
        }

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
    bool exit = game.ParseArguments(argc, argv);

    if (exit)
        return 0;

    return game.Main();
}
