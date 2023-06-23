#pragma once

#include "Gameplay/Entity.h"
#include <cstddef>
#include <memory>
#include <string>

#define MESSAGES_PATH "res/levels/messages.json"

namespace mlg {
    class AudioAsset;
}

class ChatManager : public mlg::Entity {

    struct MessagesEntry {
        std::vector<std::string> messages;
        int currentMessageIndex = 0;
    };

private:
    class LevelScene* currentLevelScene;

    std::unordered_map<std::string, MessagesEntry> messagesMap;

    explicit ChatManager(
            size_t id,
            std::string name,
            bool isStatic,
            mlg::Transform* parent);

    std::string welcomeKey = "welcome";

    float minDelayBetweenRandomMessages = 10.f;
    float maxDelayBetweenRandomMessages = 20.f;
    size_t randomMessageTimer = 0;

    std::shared_ptr<mlg::AudioAsset> messageSound;

    bool taskMessagesEnabled = true;
    bool randomMessagesEnabled = true;
    bool welcomeMessageEnabled = true;

    eventpp::CallbackList<void(const struct TaskData&)>::Handle onTaskFailedHandle;
    eventpp::CallbackList<void(int)>::Handle onProductSoldHandle;
public:
    static std::shared_ptr<ChatManager> Create(
            size_t id,
            std::string name,
            bool isStatic,
            mlg::Transform* parent,
            LevelScene* levelScene);

    ~ChatManager() override = default;

    void ParseJson();

    void SetWelcome(const std::string& key);

    void Start() override;
    void Update() override;
    void Stop() override;


    void DisableRandomMessages();
    void DisableTaskMessages();
    void DisableWelcomeMessage();

    void NewMessage(const std::string& key, float duration = 5.f);

private:
    void StartRandomMessageTimer();

    const std::string& GetMessage(const std::string& key);

    void ShowWelcomeMessage();
    void ShowMessage(const std::string& message, float duration = 5.f);
};