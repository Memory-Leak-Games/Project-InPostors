#include "UI/Components/CharacterSelector.h"

#include "Core/HID/Input.h"
#include "UI/Components/Label.h"
#include <spdlog/spdlog.h>

mlg::CharacterSelector::CharacterSelector(
        std::weak_ptr<Entity> owner, std::string name,
        const std::shared_ptr<MaterialAsset>& defaultMaterial,
        const std::shared_ptr<MaterialAsset>& focusMaterial,
        const std::shared_ptr<class FontAsset>& font)
    : Button(owner, name, defaultMaterial, focusMaterial, font) {}

void mlg::CharacterSelector::Start() {
    Button::Start();

    SetCharacterByIndex(0);
}

void mlg::CharacterSelector::Update() {
    if (IsFocused())
        return;

    if (mlg::Input::IsActionJustPressed("ui_up")) {
        SetCharacterByIndex(currentCharacter - 1);
    } else if (mlg::Input::IsActionJustPressed("ui_down")) {
        SetCharacterByIndex(currentCharacter + 1);
    } else if (mlg::Input::IsActionJustPressed("ui_accept")) {
        SetCharacterByIndex(0);
    }
}

void mlg::CharacterSelector::SetCharacter(char character) {
    int index = 0;

    if (character > 'A' && character < 'Z') {
        index = character - 'A';
    } else if (character > '0' && character < '9') {
        index = character - '0' + 26;
    } else if (character == '-') {
        index = 36;
    }

    SetCharacterByIndex(index);
}

char mlg::CharacterSelector::GetCharacter() const {
    return availableCharacters[currentCharacter];
}

void mlg::CharacterSelector::SetCharacterByIndex(int index) {
    if (index < 0)
        index = (int) availableCharacters.size() - 1;

    currentCharacter = index % (int) availableCharacters.size();
    GetLabel().lock()->SetText(std::string(1, GetCharacter()));
}
