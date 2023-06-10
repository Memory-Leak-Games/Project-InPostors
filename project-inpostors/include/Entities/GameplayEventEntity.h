#pragma once

#include "Gameplay/Entity.h"

class GameplayEventEntity : public mlg::Entity {

    GameplayEventEntity(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent);
public:
    ~GameplayEventEntity() override;

    static std::shared_ptr<GameplayEventEntity> Create(
            uint64_t id, const std::string& name,
            bool isStatic, mlg::Transform* parent,
            const class GameplayEvent& event);


private:
    void AddMesh(const GameplayEvent& event);
    void AddCollider(const GameplayEvent& event);

};