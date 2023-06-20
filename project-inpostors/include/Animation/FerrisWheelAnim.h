#pragma once

#include "Animation/AnimatedEntity.h"
#include "Levels/LevelGenerator.h"

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
}

class FerrisWheelAnim : public AnimatedEntity {
private:
    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;
    std::weak_ptr<mlg::StaticMeshComponent> staticMeshComponent;

    FerrisWheelAnim(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);

public:
    static std::shared_ptr<FerrisWheelAnim> Create(uint64_t id, const std::string& name,
                                                   bool isStatic, mlg::Transform* parent);

    void Start() override;
    void Update() override;

    ~FerrisWheelAnim() override = default;
};
