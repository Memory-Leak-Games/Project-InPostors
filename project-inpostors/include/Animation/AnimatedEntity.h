//#pragma once
//
//#include "Gameplay/Entity.h"
//#include "Levels/LevelGenerator.h"
//
//struct AnimatedEntityData {
//    int id;
//    mlg::MapObject* MapObject;
//    glm::ivec2 position;
//    float rotation;
//};
//
//class AnimatedEntity : public mlg::Entity {
//private:
//    AnimatedEntityData* animatedEntityData;
//
//protected:
//    explicit AnimatedEntity(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent);
//
//public:
//    static std::shared_ptr<AnimatedEntity> Create(uint64_t id, const std::string& name,
//                                                   bool isStatic, mlg::Transform* parent);
//
//    void Start() override;
//    void Update() override;
//
//    ~AnimatedEntity() override = default;
//};
