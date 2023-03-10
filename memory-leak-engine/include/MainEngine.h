#pragma once

#include <memory>

#include <cstdint>
#include <GLFW/glfw3.h>

#include "glm/vec3.hpp"
#include "Nodes/Node.h"
#include "glm/gtc/constants.hpp"
#include "Mesh/ModelRenderer.h"

class MainEngine {
private:

    GLFWwindow* window;

    class CameraNode* currentCamera;
    class std::shared_ptr<class Skybox> skybox;
    std::shared_ptr<class Lights> sceneLight;
    Node sceneRoot;
    ModelRenderer renderer;
public:
    explicit MainEngine();
    virtual ~MainEngine();

    int32_t Init();
    void PrepareScene();
    int32_t MainLoop();

    GLFWwindow* GetWindow() const;

    unsigned int GetSkyboxTextureId();
    friend class CameraNode;
private:
    void Stop();

    static void GLFWErrorCallback(int error, const char* description);
    int32_t InitializeWindow(const std::string& WindowName);
    void InitializeImGui(const char* glslVersion);
    void CheckGLErrors();
};
