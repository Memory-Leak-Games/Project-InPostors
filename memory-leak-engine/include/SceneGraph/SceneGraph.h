#pragma once

namespace mlg {

    class SceneGraph {
    private:
        static SceneGraph* instance;

        std::unique_ptr<class Transform> root;
    public:
        static void Initialize();
        static void Stop();

        static void CalculateGlobalTransforms();

        static SceneGraph* GetInstance();
        static Transform* GetRoot();
    };

} // mlg
