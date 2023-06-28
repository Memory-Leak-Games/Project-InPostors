#include "Animation/AnimationLibrary.h"

#include "Animation/Spawners/BurgerAnimSpawner.h"
#include "Animation/Spawners/CaffeeAnimSpawner.h"
#include "Animation/Spawners/DonutAnimSpawner.h"
#include "Animation/Spawners/FanAnimSpawner.h"
#include "Animation/Spawners/FerrisWheelAnimSpawner.h"
#include "Animation/Spawners/FriesAnimSpawner.h"

AnimationLibrary* AnimationLibrary::instance;

AnimationLibrary::AnimationLibrary() {
    animMap["ferris_wheel"] = std::make_shared<FerrisWheelAnimSpawner>();
    animMap["fan"] = std::make_shared<FanAnimSpawner>();
    animMap["donut"] = std::make_shared<DonutAnimSpawner>();
    animMap["burger"] = std::make_shared<BurgerAnimSpawner>();
    animMap["fries"] = std::make_shared<FriesAnimSpawner>();
    animMap["caffee"] = std::make_shared<CaffeeAnimSpawner>();
}

std::shared_ptr<AnimationSpawner> AnimationLibrary::Get(const std::string& id) {
    if (instance == nullptr)
        instance = new AnimationLibrary();

    MLG_ASSERT_MSG(instance->animMap.contains(id), "Animation with this id, doesn't exist");

    return instance->animMap[id];
}