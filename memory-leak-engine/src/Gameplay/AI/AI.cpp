#include "Gameplay/AI/AI.h"

#include "Core/Time.h"
#include "Gameplay/Entity.h"

#include "Macros.h"

namespace mlg {
    AI* AI::instance;

    AI* AI::GetInstance() {
        return instance;
    }

    void AI::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing AI Engine");

        instance = new AI();
    }

    void AI::Stop() {
        if (instance == nullptr)
            return;

        SPDLOG_INFO("Stopping AI Engine");

        delete instance;
    }

    void AI::AddEntity(std::weak_ptr<Entity> entity) {
        instance->entities.push_back(entity);
    }

    void AI::RemoveEntity(std::weak_ptr<Entity> entity) {
        instance->entities.erase(std::remove_if(instance->entities.begin(), instance->entities.end(),
                                                [&entity](const std::weak_ptr<Entity>& entry){
                                                    return entity.lock().get() == entry.lock().get();
                                                }), instance->entities.end());
    }

    void AI::TickAITimeStep() {
        float deltaTime = Time::GetDeltaSeconds();

        instance->timeAccumulator += deltaTime;

        while (instance->timeAccumulator >= Time::GetAITimeStep()) {
            instance->OnAIUpdate();

            //TODO: Do AI stuff here

            instance->timeAccumulator -= Time::GetAITimeStep();
        }
    }
}// namespace mlg