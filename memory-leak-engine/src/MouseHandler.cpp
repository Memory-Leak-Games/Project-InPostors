#include "MouseHandler.h"
#include "LoggingMacros.h"

using namespace mlg;

glm::vec2 MouseHandler::deltaMousePosition;
glm::vec2 MouseHandler::lastMousePosition;
glm::vec2 MouseHandler::mousePosition;

void MouseHandler::MouseCallback(GLFWwindow* window, double positionX, double positionY)
{
    mousePosition = glm::vec2(positionX, positionY);
}

glm::vec2 MouseHandler::GetDeltaMousePosition()
{
    return deltaMousePosition;
}

void MouseHandler::UpdateMouse()
{
    deltaMousePosition = mousePosition - lastMousePosition;
    lastMousePosition = mousePosition;
}


