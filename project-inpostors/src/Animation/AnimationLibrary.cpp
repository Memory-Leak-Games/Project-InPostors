#include "Animation/AnimationLibrary.h"

#include "Animation/Spawners/DonutAnimSpawner.h"
#include "Animation/Spawners/FanAnimSpawner.h"
#include "Animation/Spawners/FerrisWheelAnimSpawner.h"

AnimationLibrary* AnimationLibrary::instance;

AnimationLibrary::AnimationLibrary() {
    animMap["ferris_wheel"] = std::make_shared<FerrisWheelAnimSpawner>();
    animMap["fan"] = std::make_shared<FanAnimSpawner>();
    animMap["donut"] = std::make_shared<DonutAnimSpawner>();
}

std::shared_ptr<AnimationSpawner> AnimationLibrary::Get(const std::string& id) {
    if (instance == nullptr)
        instance = new AnimationLibrary();

    MLG_ASSERT_MSG(instance->animMap.contains(id), "Animation with this id, doesn't exist");

    return instance->animMap[id];
}