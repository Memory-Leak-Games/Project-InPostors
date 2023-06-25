#pragma once

class AnimationSpawner;

class AnimationLibrary {
private:
    static AnimationLibrary* instance;

    std::unordered_map<std::string, std::shared_ptr<AnimationSpawner>> animMap;

public:
    AnimationLibrary();

    static std::shared_ptr<AnimationSpawner> Get(const std::string& id);
};
