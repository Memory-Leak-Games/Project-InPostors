#pragma once

namespace mlg {
    class UIController {
    private:
        static UIController* instance;
        UIController();

    public:
        std::weak_ptr<class UIFocusableComponent> focused;

        static void Initialize();
        static void Stop();
        static UIController* GetInstance();
        static void Update();
    };
}