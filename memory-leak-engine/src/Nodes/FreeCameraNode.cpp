#include "Nodes/FreeCameraNode.h"
#include "MainEngine.h"
#include "MouseHandler.h"
#include "LoggingMacros.h"

FreeCameraNode::FreeCameraNode(MainEngine* engine)
: CameraNode(engine), velocity(0.f) {

}

void FreeCameraNode::Update(struct MainEngine* engine, float seconds, float deltaSeconds) {
    CameraNode::Update(engine, seconds, deltaSeconds);

    HandleMovement(deltaSeconds);
    HandleRotation();
}

void FreeCameraNode::HandleMovement(float deltaSeconds) {
    glm::vec3 MovementInput = GetMovementInput();

    if (glfwGetMouseButton(engine->GetWindow(), GLFW_MOUSE_BUTTON_2) != GLFW_PRESS)
    {
        MovementInput = glm::vec3(0.f);
    }

    glm::vec3 MovementVector = MovementInput.z * GetForwardVector();
    MovementVector -= MovementInput.x * GetRightVector();
    MovementVector += MovementInput.y * GetUpVector();

    velocity = glm::mix(velocity, MovementVector * speed, acceleration * deltaSeconds);

    glm::vec3 NewCameraPosition = GetLocalTransform()->GetPosition();
    NewCameraPosition += velocity * deltaSeconds;
    GetLocalTransform()->SetPosition(NewCameraPosition);
}

glm::vec3 FreeCameraNode::GetMovementInput() {
    glm::vec3 MovementInput(0.f);
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
    {
        MovementInput.z += 1.f;
    }
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
    {
        MovementInput.z -= 1.f;
    }

    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
    {
        MovementInput.x += 1.f;
    }
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
    {
        MovementInput.x -= 1.f;
    }

    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        MovementInput.y += 1.f;
    }
    if (glfwGetKey(engine->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        MovementInput.y -= 1.f;
    }

    return MovementInput;
}

void FreeCameraNode::HandleRotation() {
    MouseHandler::UpdateMouse();

    if (glfwGetMouseButton(engine->GetWindow(), GLFW_MOUSE_BUTTON_2) != GLFW_PRESS)
    {
        glfwSetInputMode(engine->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    glfwSetInputMode(engine->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glm::vec2 MouseInput = MouseHandler::GetDeltaMousePosition();

    glm::quat deltaPitch = glm::angleAxis(glm::radians(MouseInput.y * mouseSensitivity), glm::vec3(1, 0, 0));
    glm::quat deltaYaw = glm::angleAxis(glm::radians(-MouseInput.x * mouseSensitivity), glm::vec3(0, 1, 0));

    glm::quat newRotation = deltaYaw * GetLocalTransform()->GetRotation() * deltaPitch;
    GetLocalTransform()->SetRotation(newRotation);
}
