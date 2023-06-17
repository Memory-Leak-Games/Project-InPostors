#pragma once

#include "UI/Components/UIComponent.h"
#include <glm/fwd.hpp>

namespace mlg {

    class Spacer : public UIComponent {
    private:
        glm::vec2 size = {0.0f, 0.0f};

    public:
        explicit Spacer(std::weak_ptr<Entity> owner, std::string name);

        void SetSize(const glm::vec2& size);
        [[nodiscard]] glm::vec2 GetSize() const override;
    };

    inline Spacer::Spacer(std::weak_ptr<Entity> owner, std::string name)
        : UIComponent(owner, name) { }

    inline void Spacer::SetSize(const glm::vec2& size) {
        this->size = size;
    }

    inline glm::vec2 Spacer::GetSize() const {
        return size;
    }

}// namespace mlg