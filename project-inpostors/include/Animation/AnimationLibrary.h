#pragma once

namespace mlg {
    class Entity;
}

class AnimatedEntity;

class AnimationLibrary {
private:
    static AnimationLibrary* instance;

    std::unordered_map<std::string, std::shared_ptr<mlg::Entity>> animMap;

public:
    AnimationLibrary();

    static std::shared_ptr<mlg::Entity> Get(const std::string& id);
};
