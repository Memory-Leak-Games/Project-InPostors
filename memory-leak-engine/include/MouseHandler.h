#pragma once

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

class MouseHandler
{
private:
    static glm::vec2 mousePosition;
    static glm::vec2 lastMousePosition;
    static glm::vec2 deltaMousePosition;

    MouseHandler() = default;
public:
    static void MouseCallback(GLFWwindow* window, double positionX, double positionY);
    static void UpdateMouse();

    static glm::vec2 GetDeltaMousePosition();
};
