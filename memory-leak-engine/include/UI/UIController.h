#pragma once

namespace mlg {
    class UIController {
    private:
        static UIController* instance;
        UIController();

    public:
        class UIFocusableComponent* focused = nullptr;

        static void Initialize();
        static void Stop();
        static UIController* GetInstance();
        static void Update();
    };
}