#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {
private:
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;


    bool isDirty;
public:
    Transform();
    Transform(Transform* originalTransform);

    [[nodiscard]] glm::vec3 GetPosition() const;
    [[nodiscard]] glm::quat GetRotation() const;
    [[nodiscard]] glm::vec3 GetScale() const;

    void SetPosition(const glm::vec3& newPosition);
    void SetRotation(const glm::quat& newRotation);
    void SetScale(const glm::vec3& newScale);

    [[nodiscard]] glm::mat4 GetMatrix() const;

    friend class Node;
};
