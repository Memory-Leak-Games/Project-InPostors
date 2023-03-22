#include "Core/OldTransform.h"

using namespace mlg;

glm::mat4 OldTransform::GetMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.f), position);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), scale);
    return translation * glm::toMat4(rotation) * scaleMat;
}

glm::vec3 OldTransform::GetPosition() const {
    return position;
}

glm::quat OldTransform::GetRotation() const {
    return rotation;
}

glm::vec3 OldTransform::GetScale() const {
    return scale;
}

void OldTransform::SetPosition(const glm::vec3& newPosition) {
    position = newPosition;
    isDirty = true;
}

void OldTransform::SetScale(const glm::vec3& newScale) {
    scale = newScale;
    isDirty = true;
}

OldTransform::OldTransform() : position(glm::vec3(0.f)), rotation(glm::mat4(1.f)), scale(glm::vec3(1.f)), isDirty(true) {}

OldTransform::OldTransform(OldTransform* originalTransform) :
        position(originalTransform->position),
        rotation(originalTransform->rotation),
        scale(originalTransform->scale) {
}

void OldTransform::SetRotation(const glm::quat &newRotation) {
    rotation = newRotation;
    isDirty = true;
}
