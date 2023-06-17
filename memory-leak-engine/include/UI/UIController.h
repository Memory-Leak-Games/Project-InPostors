#pragma once

namespace mlg {
    class UIController {
    private:
        static UIController* instance;
        UIController();

        std::weak_ptr<class UIFocusableComponent> focused;
    public:
        static void Initialize();
        static void Stop();

        static UIController* Get();
        void Update();

        void SetFocus(const std::shared_ptr<class UIFocusableComponent>& component);
    };
}