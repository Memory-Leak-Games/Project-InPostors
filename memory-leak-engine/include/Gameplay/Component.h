#pragma once

namespace mlg {
    class Entity;

    class Component {
    private:
        std::string name;
        bool isQueuedForDeletion = false;

        std::weak_ptr<Entity> owner;

        Component() = delete;
    public:
        explicit Component(std::weak_ptr<Entity> owner, std::string name);

        virtual void Start();

        virtual void AIUpdate();
        virtual void PhysicsUpdate();
        virtual void Update();
        virtual void LateUpdate();
        virtual void Stop();

        [[nodiscard]] bool IsQueuedForDeletion() const;
        [[nodiscard]] const std::string& GetName() const;
        [[nodiscard]] const std::weak_ptr<Entity>& GetOwner() const;

        void SetName(const std::string& name);

        void QueueForDeletion();

        virtual ~Component() = 0;
    };

} // mlg
