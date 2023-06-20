#pragma once

class AnimationSystem;

class AnimationLibrary {
private:
    static AnimationLibrary* instance;

    std::unordered_map<std::string, std::shared_ptr<AnimationSystem>> animMap;

public:
    AnimationLibrary();

    static std::shared_ptr<AnimationSystem> Get(const std::string& id);
};
