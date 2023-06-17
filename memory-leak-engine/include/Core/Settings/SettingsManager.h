#pragma once

#include <nlohmann/json.hpp>

#include "Macros.h"

#define DEFAULT_SETTINGS_PATH "res/config/settings/"
#define SETTINGS_PATH "res/runtime_files/"

namespace mlg {

    enum class SettingsType {
        Debug,
        Engine,
        Video,
        Audio
    };

    class SettingsManager {
    private:
        static SettingsManager* instance;

        nlohmann::json debugSettings;
        nlohmann::json engineSettings;
        nlohmann::json videoSettings;
        nlohmann::json audioSettings;

    public:
        static void Initialize();
        static void Stop();

        static SettingsManager* GetInstance();

        template<typename T>
        static T Get(SettingsType settingsType, const std::string& id) {
            switch (settingsType) {
                case SettingsType::Debug:
                    return instance->debugSettings[id].get<T>();
                case SettingsType::Engine:
                    return instance->engineSettings[id].get<T>();
                case SettingsType::Video:
                    return instance->videoSettings[id].get<T>();
                case SettingsType::Audio:
                    return instance->audioSettings[id].get<T>();
            }

            MLG_ASSERT_MSG(false, "Unknown settings type");
        }

    private:
        void LoadSettings();
    };

} // mlg
