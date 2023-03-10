#pragma once


#include "Node.h"

class CameraNode : public Node {
private:
    std::shared_ptr<class Camera> camera;

protected:
    class MainEngine* engine;

public:
    CameraNode(MainEngine* engine);

    void Update(struct MainEngine* engine, float seconds, float deltaSeconds) override;
    void SetActive();
};
