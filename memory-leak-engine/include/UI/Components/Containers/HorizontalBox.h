#pragma once

#include "UI/Components/Containers/Container.h"

namespace mlg {
    class HorizontalBox : public Container {
    public:
        HorizontalBox(std::weak_ptr<Entity> owner, const std::string& name);
        ~HorizontalBox() override;

    protected:
        void UpdateContainer() override;

    private:
        void UpdateFocusableComponents();
    };
}