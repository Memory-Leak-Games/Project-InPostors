#pragma once

namespace mlg {
    class ParticleSystem;
}

class FXLibrary {
private:
    static FXLibrary* instance;

    std::unordered_map<std::string, std::shared_ptr<mlg::ParticleSystem>> fxMap;

public:
    FXLibrary();

public:
    static std::shared_ptr<mlg::ParticleSystem> Get(const std::string& id);
};
