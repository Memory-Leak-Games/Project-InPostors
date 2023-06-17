#pragma once

#include "UI/Components/Containers/Container.h"

namespace mlg {

    class VerticalBox : public Container {
    public:
        VerticalBox(std::weak_ptr<Entity> owner, std::string name);
        ~VerticalBox() override;

    protected:
        void UpdateContainer() override;

    private:
        void UpdateFocusableComponents();
    };

}// namespace mlg