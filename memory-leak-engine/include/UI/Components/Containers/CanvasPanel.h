#pragma once

#include "UI/Components/Containers/Container.h"
#include <glm/fwd.hpp>

namespace mlg {
    class CanvasPanel : public Container {
    private:
        glm::vec2 panelSize = {1280.f*0.75f, 720.f * 0.75f};
    
    public:
        CanvasPanel(std::weak_ptr<Entity> owner, std::string name);
        ~CanvasPanel() override;
    
        void SetSize(glm::vec2 size);
        glm::vec2 GetSize() const override;
    protected:
        void UpdateContainer() override;
        
    };
}