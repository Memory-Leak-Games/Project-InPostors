#include "Nodes/MotorcycleNode.h"
#include "include/Mesh/Model.h"
#include "Nodes/ModelNode.h"
#include "Gizmos/Arrow.h"
#include "MainEngine.h"
#include "LoggingMacros.h"
#include <array>
#include "Nodes/CameraNode.h"

MotorcycleNode::MotorcycleNode(MainEngine* engine, ModelRenderer* renderer) {
    auto modelShader = std::make_shared<ShaderWrapper>("res/shaders/instanced.vert", "res/shaders/motur_model.frag");
    auto baseModel = std::make_shared<Model>("res/models/Motur/MoturBody.obj", modelShader);
    auto steeringModel = std::make_shared<Model>("res/models/Motur/MoturSteering.obj", modelShader);
    auto wheelModel = std::make_shared<Model>("res/models/Motur/MoturWheel.obj", modelShader);

    auto root = std::make_shared<Node>();
    root->GetLocalTransform()->SetRotation(glm::rotate(glm::quat(), {0.f, glm::radians(90.f), 0.f}));
    AddChild(root);

    base = std::make_shared<ModelNode>(baseModel, renderer);
    root->AddChild(base);

    steering = std::make_shared<ModelNode>(steeringModel, renderer);
    steering->GetLocalTransform()->SetPosition({-2.3f, 3.5f, 0.f});
    root->AddChild(steering);

    frontWheel = std::make_shared<ModelNode>(wheelModel, renderer);
    frontWheel->GetLocalTransform()->SetPosition({-2.f, -2.3f, 0.f});
    steering->AddChild(frontWheel);

    backWheel = std::make_shared<ModelNode>(wheelModel, renderer);
    backWheel->GetLocalTransform()->SetPosition({3.2f, 1.4f, 0.f});
    backWheel->GetLocalTransform()->SetScale(glm::vec3(1.2f));
    root->AddChild(backWheel);

    camera = std::make_shared<CameraNode>(engine);
    camera->GetLocalTransform()->SetPosition({2.f, 6.f, 0.f});
    camera->GetLocalTransform()->SetRotation({{0.f, -glm::half_pi<float>(), 0.f}});
    root->AddChild(camera);

    velocity = 0.f;
    acceleration = 0.f;
}

void MotorcycleNode::Update(MainEngine* engine, float seconds, float deltaSeconds) {
    Node::Update(engine, seconds, deltaSeconds);

    glm::vec3 movementInput = HandleMovementInput(engine);

    HandleMovement(engine, deltaSeconds, movementInput);
    AnimateSteering(deltaSeconds, movementInput);
    AnimateWheels(deltaSeconds);
}

void MotorcycleNode::AnimateWheels(float deltaSeconds) {
    std::array wheels = {frontWheel, backWheel};

    for ( auto& wheel : wheels) {
        glm::quat newRotation = wheel->GetLocalTransform()->GetRotation();
        glm::quat deltaRotation = glm::quat(
                glm::vec3(0.f, 0.f, (velocity) * deltaSeconds));
        newRotation *= deltaRotation;
        wheel->GetLocalTransform()->SetRotation(newRotation);
    }
}

void MotorcycleNode::AnimateSteering(float deltaSeconds, const glm::vec3& movementInput) {
    float steeringAngle = glm::eulerAngles(steering->GetLocalTransform()->GetRotation()).y;
    steeringAngle = std::lerp(steeringAngle, movementInput.x * glm::radians(45.f), deltaSeconds * 5.f);
    steering->GetLocalTransform()->SetRotation(glm::quat({0.f, steeringAngle, 0.f}));
}

void MotorcycleNode::HandleMovement(MainEngine* engine, float deltaSeconds, glm::vec3 movementInput) {
    glm::vec3 position = GetLocalTransform()->GetPosition();
    position += GetForwardVector() * velocity * deltaSeconds + (acceleration / 2) * deltaSeconds * deltaSeconds;
    velocity += acceleration * deltaSeconds;
    GetLocalTransform()->SetPosition(position);

    if (movementInput.z * velocity >= 0)
        acceleration = movementInput.z * 15.f;
    else if (movementInput.z * velocity < 0) {
        acceleration = movementInput.z * 30.f;
    }

    if (movementInput.z == 0 && std::abs(velocity) > 0.1f) {
        acceleration = velocity * -0.4f;
        if (std::abs(velocity) < 0.15)
            velocity = 0.f;
    }

    glm::quat newRotation = GetLocalTransform()->GetRotation();
    glm::quat deltaRotation = glm::quat(
            glm::vec3(0.f, glm::clamp(velocity / 5.f, -2.f, 2.f) * movementInput.x * deltaSeconds, 0.f));
    newRotation *= deltaRotation;
    GetLocalTransform()->SetRotation(newRotation);
}

glm::vec3 MotorcycleNode::HandleMovementInput(MainEngine* engine) {
    glm::vec3 MovementInput(0.f);
    if (!isActive)
        return MovementInput;

    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        MovementInput.z += 1.f;
    }
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        MovementInput.z -= 1.f;
    }

    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        MovementInput.x += 1.f;
    }
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        MovementInput.x -= 1.f;
    }

    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        MovementInput.y += 1.f;
    }
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        MovementInput.y -= 1.f;
    }

    return MovementInput;
}

void MotorcycleNode::SetIsActive(bool isActive) {
    MotorcycleNode::isActive = isActive;
    if (isActive)
        camera->SetActive();
}


