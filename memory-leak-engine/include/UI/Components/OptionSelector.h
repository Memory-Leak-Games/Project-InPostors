#pragma once

#include "UI/Components/Button.h"

namespace mlg {

    class OptionSelector : public Button {
    public:
        enum class Direction {
            Horizontal,
            Vertical
        };

    private:
        std::vector<std::string> options;
        int currentOption = 0;

        Direction direction = Direction::Horizontal;

        std::string nextAction = "ui_right";
        std::string prevAction = "ui_left";

    public:
        OptionSelector(std::weak_ptr<Entity> owner, std::string name,
                       const std::shared_ptr<MaterialAsset>& defaultMaterial,
                       const std::shared_ptr<MaterialAsset>& focusMaterial,
                       const std::shared_ptr<class FontAsset>& font);

        template<typename Iterator>
        void AddOptions(Iterator begin, Iterator end) {
            options.insert(options.end(), begin, end);
        }

        void AddOption(const std::string& option) {
            options.push_back(option);
        }

        void SetDirection(Direction direction);

        void Start() override;
        void Update() override;

        void SetOption(int index);
        int GetOption() const;

    };


}// namespace mlg