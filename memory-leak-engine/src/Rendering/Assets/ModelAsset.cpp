#include "include/Rendering/Assets/ModelAsset.h"

#include "Rendering/Model.h"

#include "Macros.h"

namespace mlg {
    ModelAsset::ModelAsset(const std::string& path)
    : Asset(path) {}

    void ModelAsset::Draw() {
        model->Draw();
    }

    void ModelAsset::Load() {
        ZoneScopedN("Load ModelAsset");
        SPDLOG_DEBUG("Loading Model at path: {}", GetPath());
        model = std::make_unique<Model>(GetPath());
    }

    ModelAsset::~ModelAsset() = default;
} // mlg