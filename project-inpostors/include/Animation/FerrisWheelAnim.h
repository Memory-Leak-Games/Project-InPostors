#pragma once

#include "Gameplay/Entity.h"
#include "Animation/AnimatedEntity.h"
#include "Levels/LevelGenerator.h"

namespace mlg {
    class RigidbodyComponent;
    class StaticMeshComponent;
}

struct AnimatedEntityData {
    int id;
    mlg::MapObject* mapObject;
    glm::ivec2 position;
    glm::vec3 objectPosition;
    float rotation;
};

class FerrisWheelAnim : public mlg::Entity {
private:
    std::weak_ptr<mlg::RigidbodyComponent> rigidbodyComponent;

    AnimatedEntityData* animData;

    FerrisWheelAnim(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, AnimatedEntityData* animData);

public:
    static std::shared_ptr<FerrisWheelAnim> Create(uint64_t id, const std::string& name,
                                                   bool isStatic, mlg::Transform* parent,
                                                   AnimatedEntityData* animData);

    void Start() override;
    void Update() override;

    ~FerrisWheelAnim() override = default;
};
