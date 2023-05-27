#include <exception>
#include <iostream>
#include <cxxopts.hpp>
#include <memory>
#include <string>
#include <utility>

#include "Core/SceneManager/SceneManager.h"
#include "Core/Core.h"

#include "Initializer.h"

#include "Scenes/LevelScene.h"
#include "Scenes/TestScene.h"

class ProjectInpostors {
public:
    ProjectInpostors() = default;
    std::unique_ptr<mlg::Scene> startupScene;

    bool ParseArguments(int argc, char* argv[]) {
        cxxopts::Options options("Project Inpostors", "A game about couriers");
        options.allow_unrecognised_options();
        options.add_options()
            ("h,help", "Show this help")
            ("test-scene", "Run test scene")
            ("l,level", "provide level path", cxxopts::value<std::string>());

        auto resutl = options.parse(argc, argv);

        if (resutl.count("help")) {
            std::cout << options.help() << std::endl;
            return true;
        }

        if (resutl.count("test-scene")) {
            startupScene = std::move(std::make_unique<TestScene>());

            return false;
        }

        if (resutl.count("level")) {
            std::string path = resutl["level"].as<std::string>();

            auto levelScene = std::make_unique<LevelScene>(path);
            startupScene = std::move(levelScene);

            return false;
        }

        std::string path = "res/levels/maps/detroit.json";
        startupScene = std::move(std::make_unique<LevelScene>(path));

        return false;
    }


    int Main() {

        mlg::Initializer::InitializeCoreComponents();
        mlg::Initializer::InitializeSceneComponents();

        mlg::Core* engine = mlg::Core::GetInstance();

        mlg::SceneManager::LoadScene(std::move(startupScene));
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
    bool exit = game.ParseArguments(argc, argv);

    if (exit)
        return 0;

    return game.Main();
}
