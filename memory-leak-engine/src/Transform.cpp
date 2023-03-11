#include "Transform.h"

using namespace mlg;

glm::mat4 Transform::GetMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.f), position);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), scale);
    return translation * glm::toMat4(rotation) * scaleMat;
}

glm::vec3 Transform::GetPosition() const {
    return position;
}

glm::quat Transform::GetRotation() const {
    return rotation;
}

glm::vec3 Transform::GetScale() const {
    return scale;
}

void Transform::SetPosition(const glm::vec3& newPosition) {
    position = newPosition;
    isDirty = true;
}

void Transform::SetScale(const glm::vec3& newScale) {
    scale = newScale;
    isDirty = true;
}

Transform::Transform() : position(glm::vec3(0.f)), rotation(glm::mat4(1.f)), scale(glm::vec3(1.f)), isDirty(true) {}

Transform::Transform(Transform* originalTransform) :
        position(originalTransform->position),
        rotation(originalTransform->rotation),
        scale(originalTransform->scale) {
}

void Transform::SetRotation(const glm::quat &newRotation) {
    rotation = newRotation;
    isDirty = true;
}
