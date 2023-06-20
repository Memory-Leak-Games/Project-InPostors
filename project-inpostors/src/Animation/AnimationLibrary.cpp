#include "Animation/AnimationLibrary.h"

#include "Animation/AnimatedEntity.h"
#include "Animation/FerrisWheelAnim.h"

AnimationLibrary* AnimationLibrary::instance;

AnimationLibrary::AnimationLibrary() {
    animMap["ferris_wheel"] = std::make_shared<FerrisWheelAnim>();
}

std::shared_ptr<AnimatedEntity> AnimationLibrary::Get(const std::string& id) {
    if (instance == nullptr)
        instance = new AnimationLibrary();

    MLG_ASSERT_MSG(instance->animMap.contains(id), "Animation with this id, doesn't exist");

    //    return instance->animMap[id]->Clone();
    return nullptr;
}