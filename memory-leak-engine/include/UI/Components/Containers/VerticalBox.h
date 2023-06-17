#pragma once

#include "UI/Components/Containers/Container.h"
#include <glm/fwd.hpp>

namespace mlg {

    class VerticalBox : public Container {
    public:
        VerticalBox(std::weak_ptr<Entity> owner, std::string name);
        ~VerticalBox() override;

    protected:
        void UpdateContainer() override;
        glm::vec2 GetSize() const override;

    private:
        void UpdateFocusableComponents();
    };

}// namespace mlg