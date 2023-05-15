#include "FX/FXLibrary.h"

#include "FX/SmokeFX.h"
#include "FX/FactorySmokeFX.h"

FXLibrary* FXLibrary::instance;

FXLibrary::FXLibrary() {
    fxMap["smoke"] = std::make_shared<SmokeFX>();
    fxMap["factorySmoke"] = std::make_shared<FactorySmokeFX>();
}

std::shared_ptr<mlg::ParticleSystem> FXLibrary::Get(const std::string& id) {
    if (instance == nullptr)
        instance = new FXLibrary();

    MLG_ASSERT_MSG(instance->fxMap.contains(id), "FX with this id, doesnt exist");

    return instance->fxMap[id]->Clone();
}
