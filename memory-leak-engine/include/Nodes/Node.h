#pragma once

#include <vector>
#include <memory>

#include "Transform.h"

namespace mlg {
    class Node {
    private:
        std::shared_ptr<Transform> localTransform;
        glm::mat4 worldTransformMatrix;

        Node *parent{};
        std::vector<std::shared_ptr<Node>> childrenList;

        bool wasDirty{};
    public:
        explicit Node();

        void CalculateWorldTransform();

        void Draw();

        virtual void Update(class CoreEngine *engine, float seconds, float deltaSeconds);

        [[nodiscard]] virtual std::shared_ptr<Node> Clone() const;

        void AddChild(std::shared_ptr<Node> newChild);

        const std::vector<std::shared_ptr<Node>> &GetChildrenList() const;

        Transform *GetLocalTransform();

        glm::vec3 GetWorldPosition() const;

        glm::vec3 GetForwardVector() const;

        glm::vec3 GetRightVector() const;

        glm::vec3 GetUpVector() const;

        const glm::mat4 *GetWorldTransformMatrix() const;

        [[nodiscard]] bool WasDirtyThisFrame() const;


        template<typename Predicate>
        void GetAllNodes(std::vector<Node *> &foundArray, Predicate predicate);

        template<typename Predicate>
        Node *GetChild(Predicate predicate);

        Node *GetParent() const;

    protected:
        virtual void Draw(glm::mat4 &parentTransform, bool isDirty);

        void CalculateWorldTransform(glm::mat4 &parentTransform, bool isDirty);
    };

    template<typename Predicate>
    void Node::GetAllNodes(std::vector<Node *> &foundArray, Predicate predicate) {
        if (predicate(this))
            foundArray.push_back(this);

        for (const auto &node: childrenList) {
            node->GetAllNodes(foundArray, predicate);
        }
    }

    template<typename Predicate>
    Node *Node::GetChild(Predicate predicate) {
        for (std::shared_ptr<Node> child: childrenList) {
            if (predicate(child.get()))
                return child.get();
        }
        return nullptr;
    }
}
