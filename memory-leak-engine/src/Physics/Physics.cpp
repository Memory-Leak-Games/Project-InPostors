#include "Physics/Physics.h"

#include "Core/Time.h"
#include "Physics/Rigidbody.h"

#include "Physics/CollisionManager.h"

#include "Macros.h"
#include <cstddef>

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
        instance = nullptr;

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
        float deltaTime = Time::GetDeltaSeconds();

        instance->timeAccumulator += deltaTime;

        // skip very long frames
        if (deltaTime > 0.25f)
            return;

        while (instance->timeAccumulator >= Time::GetFixedTimeStep()) {
            ZoneScopedNC("Tick Physics", tracy::Color::ColorType::Green);
            instance->OnFixedUpdate();
            instance->SolveDynamics();

            CollisionManager::UpdateSpacialGrid();
            CollisionManager::DetectCollisions();
            CollisionManager::SolveCollisions();

            instance->timeAccumulator -= Time::GetFixedTimeStep();
        }
    }

    void Physics::SolveDynamics() {
        ZoneScopedC(tracy::Color::ColorType::Green);

        for (auto& rigidbody : states) {
            if (rigidbody.expired())
                continue;

            if (rigidbody.lock()->isKinematic)
                continue;

            rigidbody.lock()->Integrate();
            rigidbody.lock()->UpdateColliders();
        }
    }

} // mlg