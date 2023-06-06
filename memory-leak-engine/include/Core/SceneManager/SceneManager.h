#pragma once

namespace mlg {

    class SceneManager {
    private:
        static SceneManager* instance;

        std::unique_ptr<class Scene> currentScene;

        SceneManager() = default;
    public:
        ~SceneManager();

        static void Initialize();
        static void Stop();

        static Scene* GetCurrentScene();

        static void LoadScene(std::unique_ptr<class Scene> scene);
        static void Update();

        static void ExitGame();
    };

}// namespace mlg