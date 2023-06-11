#pragma once

#include "Core/SceneManager/LoadingScreen.h"
namespace mlg {

    class SceneManager {
    private:
        static SceneManager* instance;

        std::unique_ptr<class LoadingScreen> loadingScreen;

        std::unique_ptr<class Scene> nextScene;
        std::unique_ptr<class Scene> currentScene;

        SceneManager();
    public:
        ~SceneManager();

        static void Initialize();
        static void Stop();

        static Scene* GetCurrentScene();

        static void SetNextScene(std::unique_ptr<class Scene> scene);
        static void LoadNextScene();
        static bool HasNextScene();

        static void Update();

        static void ExitGame();
    };

}// namespace mlg