#include "Core/Settings/SettingsManager.h"

#include <filesystem>
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
        std::string path;

        if (std::filesystem::exists(SETTINGS_PATH "debug.json"))
            path = SETTINGS_PATH "debug.json";
        else
            path = DEFAULT_SETTINGS_PATH "debug.json";

        std::ifstream settingsFile{path};
        debugSettings = json::parse(settingsFile);
        settingsFile.close();


        if (std::filesystem::exists(SETTINGS_PATH "engine.json"))
            path = SETTINGS_PATH "engine.json";
        else
            path = DEFAULT_SETTINGS_PATH "engine.json";

        settingsFile.open(path);
        engineSettings = json::parse(settingsFile);
        settingsFile.close();

        if (std::filesystem::exists(SETTINGS_PATH "video.json"))
            path = SETTINGS_PATH "video.json";
        else
            path = DEFAULT_SETTINGS_PATH "video.json";

        settingsFile.open(path);
        videoSettings = json::parse(settingsFile);
        settingsFile.close();

        if (std::filesystem::exists(SETTINGS_PATH "audio.json"))
            path = SETTINGS_PATH "audio.json";
        else
            path = DEFAULT_SETTINGS_PATH "audio.json";

        settingsFile.open(path);
        audioSettings = json::parse(settingsFile);
        settingsFile.close();
    }
} // mlg