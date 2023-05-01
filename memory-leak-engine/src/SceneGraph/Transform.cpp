#include "SceneGraph/Transform.h"

#include "Core/Math.h"

namespace mlg {
    Transform::Transform()
            : position(0.f), rotation(glm::vec3(0.f)), scale(1.f),
              isDirty(true), isDirtyLocal(true), worldMatrix(1.f), localMatrix(1.f) {
    }

    const glm::vec3& Transform::GetPosition() const {
        return position;
    }

    void Transform::SetPosition(const glm::vec3& position) {
        if (position == this->position)
            return;

        onTransformationChange();
        isDirtyLocal = true;
        SetDirtyRecursive();

        Transform::position = position;
    }

    const glm::vec3& Transform::GetScale() const {
        return scale;
    }

    void Transform::SetScale(const glm::vec3& scale) {
        if (scale == this->scale)
            return;

        onTransformationChange();
        isDirtyLocal = true;
        SetDirtyRecursive();

        Transform::scale = scale;
    }

    const glm::quat& Transform::GetRotation() const {
        return rotation;
    }

    const glm::vec3 Transform::GetEulerRotation() const {
        return glm::eulerAngles(rotation);
    }

    void Transform::SetRotation(const glm::quat& rotation) {
        if (rotation == this->rotation)
            return;

        onTransformationChange();
        isDirtyLocal = true;
        SetDirtyRecursive();

        Transform::rotation = rotation;
    }

    void Transform::SetEulerRotation(const glm::vec3& rotation) {
        glm::quat rotationQuat{rotation};

        if (rotationQuat == this->rotation)
            return;

        onTransformationChange();
        isDirtyLocal = true;
        SetDirtyRecursive();

        Transform::rotation = rotationQuat;
    }

    const glm::mat4& Transform::GetWorldMatrix() {
        if (isDirty)
            ReCalculateParentRecursive();

        return worldMatrix;
    }

    const glm::mat4& Transform::GetLocalMatrix() {
        if (isDirtyLocal) {
            ZoneScopedN("Calculate Local Matrix");

            glm::mat4 translation{1.f};
            translation[3][0] = position.x;
            translation[3][1] = position.y;
            translation[3][2] = position.z;

            glm::mat4 scaleMat{1.f};
            scaleMat[0][0] = scale.x;
            scaleMat[1][1] = scale.y;
            scaleMat[2][2] = scale.z;

            localMatrix = translation * glm::toMat4(rotation) * scaleMat;
            isDirtyLocal = false;
        }

        return localMatrix;
    }

    glm::vec3 Transform::InverseDirection(const glm::vec3& direction) {
        const glm::mat4 worldMatrix = GetWorldMatrix();
        return glm::inverse((glm::mat3(worldMatrix))) * direction;
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

    const std::vector<std::weak_ptr<Transform>>& Transform::GetChildren() {
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

    void Transform::Calculate(const glm::mat4& parentMatrix, bool isParentDirty) {
        isParentDirty |= isDirty;

        if (isParentDirty) {
            worldMatrix = parentMatrix * GetLocalMatrix();
            isDirty = false;
        }

        for (const auto& child: children) {
            if (child.expired())
                continue;

            child.lock()->Calculate(worldMatrix, isParentDirty);
        }
    }

    void Transform::SetDirtyRecursive() {
        isDirty = true;

        for (const auto& child: children) {
            if (child.expired())
                continue;

            child.lock()->SetDirtyRecursive();
        }
    }

    void Transform::ReCalculateParentRecursive() {
        if (parent == nullptr) {
            Calculate();
            return;
        }

        if (parent->isDirty) {
            parent->ReCalculateParentRecursive();
            return;
        }

        Calculate(parent->worldMatrix, true);
    }

    Transform::~Transform() {
        for (auto& child: children) {
            if (child.expired())
                continue;

            child.lock()->parent = this->parent;
        }
    }

    void Transform::LookAt(const glm::vec3& lookAtPosition) {
        const glm::vec3 front = glm::normalize(lookAtPosition - position);
        const glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), front));
        const glm::vec3 up = glm::normalize(glm::cross(front, right));
        rotation = glm::quatLookAt(front, up);
    }

    bool Transform::GetIsDirty() const {
        return isDirty;
    }


} // mlg