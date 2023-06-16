#include "UI/Components/UIComponent.h"

namespace mlg {

    class Container : public UIComponent {
    protected:
        std::list<std::weak_ptr<UIComponent>> children;

    public:
        Container(std::weak_ptr<Entity> owner, std::string name);
        ~Container() override = 0;

        void Start() override;

        void AddChild(std::weak_ptr<UIComponent> child);
        void RemoveChild(std::weak_ptr<UIComponent> child);

        void SetPosition(const glm::vec2& position) override;
        void SetAnchor(const glm::vec2& anchor) override;
        glm::vec2 GetSize() const override;

        void GrabFocus();

        void SetVisible(bool visible) override;

    protected:
        virtual void UpdateContainer() = 0;
    };

}// namespace mlg