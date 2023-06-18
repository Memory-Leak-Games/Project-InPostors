#include "UI/Components/OptionSelector.h"
#include "Core/HID/Input.h"
#include "UI/Components/Button.h"
#include "UI/Components/Label.h"
#include <spdlog/fmt/bundled/format.h>
#include <string>

mlg::OptionSelector::OptionSelector(
        std::weak_ptr<Entity> owner, std::string name,
        const std::shared_ptr<MaterialAsset>& defaultMaterial,
        const std::shared_ptr<MaterialAsset>& focusMaterial,
        const std::shared_ptr<class FontAsset>& font)
    : Button(owner, name, defaultMaterial, focusMaterial, font) {
}

void mlg::OptionSelector::SetDirection(Direction direction) {
    OptionSelector::direction = direction;

    nextAction = direction == Direction::Horizontal ? "ui_right" : "ui_down";
    prevAction = direction == Direction::Horizontal ? "ui_left" : "ui_up";
}

void mlg::OptionSelector::Start() {
    Button::Start();

    SetOption(currentOption);
}

void mlg::OptionSelector::Update() {
    if (!IsFocused())
        return;

    if (mlg::Input::IsActionJustPressed(prevAction)) {
        SetOption(currentOption - 1);
    } else if (mlg::Input::IsActionJustPressed(nextAction)) {
        SetOption(currentOption + 1);
    }
}

void mlg::OptionSelector::Accept() {
    SetOption(0);
}

void mlg::OptionSelector::SetOption(int index) {
    if (options.empty()) {
        GetLabel().lock()->SetText("ERROR");
        return;
    }

    if (index < 0)
        index = options.size() - 1;

    currentOption = index % options.size();

    std::string text;

    if (direction == Direction::Horizontal)
        text = fmt::format("< {:^10} >", options[currentOption]);
    else
        text = fmt::format("{}", options[currentOption]);

    GetLabel().lock()->SetText(text);
}

int mlg::OptionSelector::GetOption() const {
    return currentOption;
}
