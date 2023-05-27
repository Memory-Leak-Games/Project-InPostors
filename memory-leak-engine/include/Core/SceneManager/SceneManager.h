#pragma once

namespace mlg {

    class SceneManager {
    private:
        static SceneManager* instance;

        std::unique_ptr<class Scene> actualScene;

        SceneManager() = default;
    public:
        ~SceneManager();

        static void Initialize();
        static void Stop();
    
        static void LoadScene(std::unique_ptr<class Scene> scene);
        static void Update();
    };

}// namespace mlg