#pragma once

namespace mlg {
    class Entity;

    class Component {
    private:
        std::string name;
        bool isQueuedForDeletion = false;

        std::weak_ptr<Entity> owner;

        explicit Component(std::weak_ptr<Entity> owner, std::string name);
    public:
        Component() = delete;

        virtual void PhysicsUpdate();
        virtual void Update();
        virtual void LateUpdate();

        const std::string& GetName() const;
        void SetName(const std::string& name);

        [[nodiscard]] bool IsQueuedForDeletion() const;

        virtual ~Component() = 0;
    };

} // mlg
