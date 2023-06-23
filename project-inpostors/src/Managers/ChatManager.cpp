
#include <fstream>
#include <memory>

#include "Audio/Assets/AudioAsset.h"
#include "Core/AssetManager/AssetManager.h"
#include "Core/TimerManager.h"
#include "Managers/ChatManager.h"
#include "Managers/TaskManager.h"
#include "Scenes/LevelScene.h"
#include "UI/GameplayOverlay.h"

using json = nlohmann::json;
using Random = effolkronium::random_static;

ChatManager::ChatManager(
        size_t id,
        std::string name,
        bool isStatic,
        mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {
    ParseJson();
    messageSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/announcement-sound.mp3");
}

std::shared_ptr<ChatManager> ChatManager::Create(
        size_t id,
        std::string name,
        bool isStatic,
        mlg::Transform* parent,
        LevelScene* levelScene) {
    auto result = std::shared_ptr<ChatManager>(
            new ChatManager(id, name, isStatic, parent));
    result->currentLevelScene = levelScene;

    return result;
}

void ChatManager::ParseJson() {
    std::ifstream file{MESSAGES_PATH};
    json messagesJson = json::parse(file);
    file.close();

    minDelayBetweenRandomMessages = messagesJson["min_delay"];
    maxDelayBetweenRandomMessages = messagesJson["max_delay"];

    for (const auto& [key, value] : messagesJson["messages"].items()) {
        std::vector<std::string> messages;
        for (const auto& message : value) {
            messages.push_back(message);
        }
        Random::shuffle(messages.begin(), messages.end());

        messagesMap.insert({key, {messages, 0}});
    }
}

void ChatManager::Start() {
    if (welcomeMessageEnabled)
        ShowWelcomeMessage();

    if (randomMessagesEnabled)
        StartRandomMessageTimer();

    if (!taskMessagesEnabled)
        return;

    TaskManager* taskManager = currentLevelScene->GetTaskManager();

    onTaskFailedHandle = taskManager->OnTaskFailed.append(
            [this](const TaskData& taskData) {
                std::string message = fmt::format(
                        fmt::runtime(GetMessage("task_failed")),
                        fmt::arg("product", taskData.productId),
                        fmt::arg("bonus", taskData.bonus),
                        fmt::arg("reward", taskData.reward));

                ShowMessage(message, 7.f);
            });
    onProductSoldHandle = taskManager->OnProductSold.append(
            [this](int price) {
                std::string message = fmt::format(
                        fmt::runtime(GetMessage("product_sold")),
                        fmt::arg("price", price));

                ShowMessage(message, 7.f);
            });
}

void ChatManager::Update() {
}

void ChatManager::Stop() {
    DisableRandomMessages();
    DisableRandomMessages();
}

void ChatManager::StartRandomMessageTimer() {
    float delay = Random::get(
            minDelayBetweenRandomMessages,
            maxDelayBetweenRandomMessages);

    if (mlg::TimerManager::Get()->IsTimerValid(randomMessageTimer)) {
        mlg::TimerManager::Get()->ClearTimer(randomMessageTimer);
    }

    randomMessageTimer = mlg::TimerManager::Get()->SetTimer(
            delay,
            false,
            [this]() {
                ShowMessage(GetMessage("random"));
                StartRandomMessageTimer();
            });
}

const std::string& ChatManager::GetMessage(const std::string& key) {
    MessagesEntry& messagesEntry = messagesMap[key];

    if (messagesEntry.currentMessageIndex >= messagesEntry.messages.size()) {
        messagesEntry.currentMessageIndex = 0;
        Random::shuffle(
                messagesEntry.messages.begin(),
                messagesEntry.messages.end());
    }

    return messagesEntry.messages[messagesEntry.currentMessageIndex++];
}

void ChatManager::ShowWelcomeMessage() {
    std::string message = fmt::format(
            fmt::runtime(GetMessage(welcomeKey)),
            fmt::arg("map_name", currentLevelScene->GetLevelName()));
    ShowMessage(message, 7.f);
}

void ChatManager::ShowMessage(const std::string& message, float duration) {
    currentLevelScene->GetGameplayOverlay()->ShowMessage(message, duration);
    messageSound->Play();
}

void ChatManager::DisableRandomMessages() {
    mlg::TimerManager::Get()->ClearTimer(randomMessageTimer);
    randomMessagesEnabled = false;
}

void ChatManager::DisableTaskMessages() {
    TaskManager* taskManager = currentLevelScene->GetTaskManager();

    taskManager->OnTaskFailed.remove(onTaskFailedHandle);
    taskManager->OnProductSold.remove(onProductSoldHandle);
    taskMessagesEnabled = false;
}

void ChatManager::DisableWelcomeMessage() {
    welcomeMessageEnabled = false;
}

void ChatManager::NewMessage(const std::string& key, float duration) {
    ShowMessage(GetMessage(key), duration);
}

void ChatManager::SetWelcome(const std::string& key) {
    welcomeKey = key;
}
