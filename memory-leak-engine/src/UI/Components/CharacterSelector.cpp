#include "UI/Components/CharacterSelector.h"

#include "UI/Components/OptionSelector.h"
#include <string>


mlg::CharacterSelector::CharacterSelector(
        std::weak_ptr<Entity> owner, std::string name,
        const std::shared_ptr<MaterialAsset>& defaultMaterial,
        const std::shared_ptr<MaterialAsset>& focusMaterial,
        const std::shared_ptr<class FontAsset>& font)
    : OptionSelector(owner, name, defaultMaterial, focusMaterial, font) {

    AddOptions(availableCharacters.begin(), availableCharacters.end());
    SetDirection(Direction::Vertical);
}

char mlg::CharacterSelector::GetCharacter() const {
    return availableCharacters[GetOption()].front();
}