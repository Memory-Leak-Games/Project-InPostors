#pragma once

#include "Gameplay/Entity.h"

namespace mlg {
    class Button;
    class Image;
    class Label;
    class CharacterSelector;
}// namespace mlg

class FinishScreen : public mlg::Entity {
private:
    std::weak_ptr<mlg::Image> background;

    // header
    std::weak_ptr<mlg::Label> headerText;

    // left panel
    std::weak_ptr<mlg::Image> leftBackground;
    std::weak_ptr<mlg::Button> exitButton;
    std::weak_ptr<mlg::Label> scoreLabel;

    // right panel
    std::weak_ptr<mlg::Image> rightBackground;
    std::weak_ptr<mlg::Label> rightHeaderText;
    std::weak_ptr<mlg::Label> scoreBoard;

    std::array<std::weak_ptr<mlg::CharacterSelector>, 4> characterSelectors;

    FinishScreen(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<FinishScreen> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);

    ~FinishScreen() override;

    void SetScore(int score, const std::string& levelName);

private:
    void CreateBackground();
    void CreateLeftPanel();
    void CreateRightPanel();

    void CreateCharacterSelectors();

    void UpdateScoreBoard(int currentScore, const std::string& levelName);
    std::string GetUserName();
};
