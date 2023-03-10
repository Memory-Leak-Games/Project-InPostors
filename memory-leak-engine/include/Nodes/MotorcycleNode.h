#pragma once


#include <memory>
#include "Node.h"
#include "MainEngine.h"

class MotorcycleNode: public Node {
private:
    std::shared_ptr<class ModelNode> base;
    std::shared_ptr<ModelNode> steering;
    std::shared_ptr<ModelNode> frontWheel;
    std::shared_ptr<ModelNode> backWheel;
    std::shared_ptr<class CameraNode> camera;

    float velocity;
    float acceleration;

    bool isActive = false;
public:
    MotorcycleNode(class MainEngine* engine, class ModelRenderer* renderer);

    void Update(struct MainEngine* engine, float seconds, float deltaSeconds) override;

    void SetIsActive(bool isActive);

private:
    glm::vec3 HandleMovementInput(MainEngine* engine);

    void HandleMovement(MainEngine* engine, float deltaSeconds, glm::vec3 movementInput);

    void AnimateSteering(float deltaSeconds, const glm::vec3& movementInput);

    void AnimateWheels(float deltaSeconds);
};
