#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class Button;
    class Image;
    class CanvasPanel;
    class Container;
}// namespace mlg

class PauseMenu : public mlg::Entity {
private:
    std::weak_ptr<mlg::CanvasPanel> menuPanel;

    bool visible = false;

    PauseMenu(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<PauseMenu> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

    void SetVisible(bool visible);

private:
    void AddResumeButton(mlg::Container* container);
    void AddExitButton(mlg::Container* container);
};