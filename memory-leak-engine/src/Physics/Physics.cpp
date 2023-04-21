#include "Physics/Physics.h"

#include "Core/Time.h"
#include "Physics/Rigidbody.h"

#include "Physics/CollisionManager.h"

#include "Macros.h"

namespace mlg {
    Physics* Physics::instance;

    Physics* Physics::GetInstance() {
        return instance;
    }

    void Physics::Initialize() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Initializing Physics Engine");

        instance = new Physics();
        CollisionManager::Initialize();
    }

    void Physics::Stop() {
        if (instance == nullptr)
            return;

        SPDLOG_INFO("Stopping Physics Engine");

        delete instance;
        CollisionManager::Stop();
    }

    void Physics::AddRigidbody(std::weak_ptr<Rigidbody> state) {
        instance->states.push_back(state);
    }

    void Physics::RemoveRigidbody(std::weak_ptr<Rigidbody> state) {
        instance->states.erase(std::remove_if(instance->states.begin(), instance->states.end(),
                                              [&state](const std::weak_ptr<Rigidbody>& entry) {
                                                  return state.lock().get() == entry.lock().get();
                                              }), instance->states.end());
    }

    void Physics::TickFixedTimeSteps() {
        ZoneScopedNC("Tick Physics", tracy::Color::ColorType::Green);

        float deltaTime = Time::GetTrueDeltaSeconds();

        instance->timeAccumulator += deltaTime;

        while (instance->timeAccumulator >= Time::GetFixedTimeStep()) {
            instance->OnFixedUpdate();
            instance->SolveDynamics();

            CollisionManager::UpdateSpacialGrid();
            CollisionManager::DetectCollisions();
            CollisionManager::SolveCollisions();

            instance->timeAccumulator -= Time::GetFixedTimeStep();
        }
    }

    void Physics::SolveDynamics() {
        for (auto& rigidbody : states) {
            if (rigidbody.expired())
                continue;

            rigidbody.lock()->Integrate();
            rigidbody.lock()->UpdateColliders();
        }
    }

} // mlg