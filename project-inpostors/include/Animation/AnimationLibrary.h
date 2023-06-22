#pragma once

class FerrisWheelAnim;

class AnimationLibrary {
private:
    static AnimationLibrary* instance;

    std::unordered_map<std::string, std::shared_ptr<FerrisWheelAnim>> animMap;

public:
    AnimationLibrary();

    static std::shared_ptr<FerrisWheelAnim> Get(const std::string& id);
};
