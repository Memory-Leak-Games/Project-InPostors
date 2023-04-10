#include "Core/Settings/SettingsManager.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Macros.h"

using json = nlohmann::json;

namespace mlg {
    SettingsManager* SettingsManager::instance;

    void SettingsManager::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing SettingsManager");

        instance = new SettingsManager();

        instance->LoadSettings();
    }

    void SettingsManager::Stop() {
        if (instance == nullptr)
            return;

        SPDLOG_INFO("Stopping SettingsManager");
        delete instance;
    }

    SettingsManager* SettingsManager::GetInstance() {
        return instance;
    }

    void SettingsManager::LoadSettings() {
        std::ifstream settingsFile{"res/config/settings/debug.json"};
        debugSettings = json::parse(settingsFile);
        settingsFile.close();

        settingsFile.open("res/config/settings/engine.json");
        engineSettings = json::parse(settingsFile);
        settingsFile.close();

        settingsFile.open("res/config/settings/video.json");
        videoSettings = json::parse(settingsFile);
    }
} // mlg