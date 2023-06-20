#pragma once

class AnimatedEntity;

class AnimationLibrary {
private:
    static AnimationLibrary* instance;

    std::unordered_map<std::string, std::shared_ptr<AnimatedEntity>> animMap;

public:
    AnimationLibrary();

    static std::shared_ptr<AnimatedEntity> Get(const std::string& id);
};
