#include <cxxopts.hpp>
#include <glm/fwd.hpp>
#include <iostream>


#include "Core/Core.h"
#include "Core/SceneManager/SceneManager.h"


#include "Core/Window.h"
#include "Initializer.h"

#include "Scenes/LevelScene.h"
#include "Scenes/MenuScene.h"
#include "Scenes/TestScene.h"

class ProjectInpostors {
public:
    ProjectInpostors() = default;
    std::unique_ptr<mlg::Scene> startupScene;

    bool windowTypeOverride = false;
    mlg::WindowType windowType = mlg::WindowType::Windowed;

    bool resolutionOverride = false;
    glm::ivec2 resolution = {1280, 720};

    bool ParseArguments(int argc, char* argv[]) {
        cxxopts::Options options("Project Inpostors", "A game about couriers");
        options.allow_unrecognised_options();
        options.add_options()("h,help", "Show this help")("test-scene", "Run test scene")("f,fullscreen", "Run in fullscreen mode (overrides config)")("w,windowed", "Run in windowed mode (overrides config)")("b,borderless", "Run in borderless mode (overrides config)")("resolution", "change window resolution (overrides config)", cxxopts::value<std::string>())("l,level", "provide level path", cxxopts::value<std::string>());

        auto result = options.parse(argc, argv);

        if (result.count("windowed")) {
            windowTypeOverride = true;
            windowType = mlg::WindowType::Windowed;
        } else if (result.count("fullscreen")) {
            windowTypeOverride = true;
            windowType = mlg::WindowType::Fullscreen;
        } else if (result.count("borderless")) {
            windowTypeOverride = true;
            windowType = mlg::WindowType::Borderless;
        }

        if (result.count("resolution")) {
            std::string resolutionString = result["resolution"].as<std::string>();
            std::string widthString = resolutionString.substr(
                    0, resolutionString.find('x'));
            std::string heightString = resolutionString.substr(
                    resolutionString.find('x') + 1, resolutionString.size());

            resolutionOverride = true;
            resolution = {std::stoi(widthString),
                          std::stoi(heightString)};
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

        mlg::Window::Get()->SetIcon("res/textures/logos/aipost_logo_icon.png");
        mlg::Window::Get()->SetTitle("AIPost™");

        if (windowTypeOverride) {
            mlg::Window::Get()->SetWindowType(windowType);
        }

        if (resolutionOverride) {
            mlg::Window::Get()->SetResolution(resolution);
        }

        mlg::Core* engine = mlg::Core::Get();

        mlg::SceneManager::SetNextScene(std::move(startupScene));

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
