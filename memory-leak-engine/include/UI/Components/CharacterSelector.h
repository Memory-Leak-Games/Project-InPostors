#pragma once

#include "UI/Components/Button.h"

namespace mlg {
    class CharacterSelector : public Button {
    private:
        constexpr static std::array<char, 37> availableCharacters = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                '-'};
        int currentCharacter = 0;

    public:
        CharacterSelector(std::weak_ptr<Entity> owner, std::string name,
                          const std::shared_ptr<MaterialAsset>& defaultMaterial,
                          const std::shared_ptr<MaterialAsset>& focusMaterial,
                          const std::shared_ptr<class FontAsset>& font);

        void Start() override;
        void Update() override;

        void SetCharacter(char character);
        char GetCharacter() const;

    private:
        void SetCharacterByIndex(int index);
    };

}// namespace mlg