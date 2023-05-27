#pragma once

namespace mlg {

    class Initializer {
    public:
        static void InitializeCoreComponents();
        static void StopCoreComponents();

        static void InitializeSceneComponents();
        static void StopSceneComponents();
    };

}// namespace mlg