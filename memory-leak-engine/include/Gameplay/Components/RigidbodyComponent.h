#pragma once

#include "Gameplay/Component.h"

#include "Physics/Colliders/ColliderShapes.h"
#include "Physics/Rigidbody.h"

namespace mlg {

    class RigidbodyComponent : public Component {
    public:
        eventpp::CallbackList<void(CollisionEvent)> OnCollisionEnter;
        eventpp::CallbackList<void(CollisionEvent)> OnTriggerEnter;

    private:
        std::shared_ptr<Rigidbody> rigidbody;

        static std::unordered_map<Rigidbody*, RigidbodyComponent*> rigidbodies;

    public:
        RigidbodyComponent(const std::weak_ptr<Entity>& owner, const std::string& name);

        static std::weak_ptr<Entity> GetColliderOwner(const Collider& collider);

        void Start() override;

        void PhysicsUpdate() override;
        void Update() override;

#ifdef DEBUG
        void DebugDraw();
#endif

        void AddForce(glm::vec2 force);
        void AddForce(glm::vec2 force, glm::vec2 localPosition);
        void AddTorque(float value);

        void SetLinearDrag(float value);
        void SetAngularDrag(float value);
        void SetAngularVelocity(float newAngularVelocity);

        glm::vec2 GetLinearVelocity();
        glm::vec3 GetLinearVelocity3D();
        glm::vec2 GetLinearAcceleration();

        glm::vec3 GetLocalVelocity();

        float GetAngularSpeed();
        float GetAngularAcceleration();

        void SetPosition(const glm::vec2& position);
        void SetRotation(float rotation);

        const glm::vec2& GetPosition();
        float GetRotation();

        template<typename T, typename ... Args>
        std::weak_ptr<Collider> AddCollider(Args&& ... args) {
            auto shape = std::make_unique<T>(rigidbody->position, std::forward<Args>(args) ...);
            return AddCollider(std::move(shape));
        }

        template<typename T, typename ... Args>
        std::weak_ptr<Collider> AddTrigger(Args&& ... args) {
            auto shape = std::make_unique<T>(rigidbody->position, std::forward<Args>(args) ...);
            return AddTrigger(std::move(shape));
        }

        void OverlapCircle(float radius, std::vector<std::weak_ptr<Entity>>& output);
        void GetOverlappingColliders(std::vector<std::weak_ptr<Collider>>& output);

        void SetKinematic(bool isKinematic);
        void SetMass(float mass);
        void SetBounciness(float bounciness);

        ~RigidbodyComponent() override;

    private:
        std::weak_ptr<Collider> AddCollider(std::unique_ptr<ColliderShape::Shape> shape);
        std::weak_ptr<Collider> AddTrigger(std::unique_ptr<ColliderShape::Shape> shape);
    };

} // mlg
