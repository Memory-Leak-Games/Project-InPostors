#include "include/SceneGraph/Transform.h"

namespace mlg {
    Transform::Transform()
            : position(0.f), rotation(glm::vec3(0.f)), scale(1.f),
              isDirty(true), worldMatrix(1.f), localMatrix(1.f) {
    }

    Transform::Transform(const Transform& original)
            : position(0.f), rotation(glm::vec3(0.f)), scale(1.f),
              isDirty(true), worldMatrix(1.f), localMatrix(1.f) {

    }

    void Transform::operator=(const Transform& another) {
        if (this == &another)
            return;

        this->position = another.position;
        this->rotation = another.rotation;
        this->scale = another.scale;

        this->localMatrix = another.localMatrix;

        isDirty = true;
    }

    const glm::vec3& Transform::GetPosition() const {
        return position;
    }

    void Transform::SetPosition(const glm::vec3& position) {
        SetDirtyRecursive();
        Transform::position = position;
    }

    const glm::vec3& Transform::GetScale() const {
        return scale;
    }

    void Transform::SetScale(const glm::vec3& scale) {
        SetDirtyRecursive();
        Transform::scale = scale;
    }

    const glm::quat& Transform::GetRotation() const {
        return rotation;
    }

    void Transform::SetRotation(const glm::quat& rotation) {
        SetDirtyRecursive();
        Transform::rotation = rotation;
    }

    const glm::mat4& Transform::GetWorldMatrix() {
        if (isDirty)
            ReCalculateParentRecursive();

        return worldMatrix;
    }

    const glm::mat4& Transform::GetLocalMatrix() {
        if (isDirty) {
            glm::mat4 translation = glm::translate(glm::mat4(1.f), position);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), scale);
            localMatrix = translation * glm::toMat4(rotation) * scaleMat;
        }

        return localMatrix;
    }

    glm::vec3 Transform::GetWorldPosition() {
        if (isDirty)
            ReCalculateParentRecursive();

        float x = worldMatrix[3][0];
        float y = worldMatrix[3][1];
        float z = worldMatrix[3][2];

        return {x, y, z};
    }

    glm::vec3 Transform::GetForwardVector() {
        if (isDirty)
            ReCalculateParentRecursive();

        glm::vec3 result;
        result.x = worldMatrix[2][0];
        result.y = worldMatrix[2][1];
        result.z = worldMatrix[2][2];

        return glm::normalize(result);
    }

    glm::vec3 Transform::GetRightVector() {
        if (isDirty)
            ReCalculateParentRecursive();

        glm::vec3 result;
        result.x = -worldMatrix[0][0];
        result.y = -worldMatrix[0][1];
        result.z = -worldMatrix[0][2];

        return glm::normalize(result);
    }

    glm::vec3 Transform::GetUpVector() {
        if (isDirty)
            ReCalculateParentRecursive();

        glm::vec3 result;
        result.x = worldMatrix[1][0];
        result.y = worldMatrix[1][1];
        result.z = worldMatrix[1][2];

        return glm::normalize(result);
    }

    const std::vector<std::shared_ptr<Transform>>& Transform::GetChildren() {
        return children;
    }

    Transform* Transform::GetParent() {
        return parent;
    }

    void Transform::AddChild(const std::shared_ptr<Transform>& newChild) {
        if (newChild.get() == this || newChild.get() == parent)
            return;

        newChild->parent = this;
        children.push_back(newChild);
        newChild->isDirty = true;
    }

    void Transform::Calculate() {
        Calculate(GetLocalMatrix(), isDirty);
    }

    void Transform::Calculate(const glm::mat4& parentMatrix, bool isDirtyLocal) {
        isDirtyLocal |= isDirty;

        if (isDirtyLocal) {
            worldMatrix = parentMatrix * GetLocalMatrix();
            isDirty = false;
        }

        for (const std::shared_ptr<Transform>& child : children) {
            child->Calculate(worldMatrix, isDirtyLocal);
        }
    }

    void Transform::SetDirtyRecursive() {
        isDirty = true;

        for (const std::shared_ptr<Transform>& child : children) {
            child->SetDirtyRecursive();
        }
    }

    void Transform::ReCalculateParentRecursive() {
        if (parent == nullptr)
        {
            Calculate();
            return;
        }

        if (parent->isDirty) {
            ReCalculateParentRecursive();
            return;
        }

        Calculate(parent->worldMatrix, true);
    }

    Transform::~Transform() {
        for (auto& child : children) {
            child->parent = this->parent;
        }
    }


} // mlg