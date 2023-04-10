#pragma once

namespace mlg {

    class SceneGraph {
    private:
        static class Transform* root;
    public:
        static void Initialize();
        static void Stop();

        static void CalculateGlobalTransforms();

        static Transform* GetRoot();
    };

} // mlg
