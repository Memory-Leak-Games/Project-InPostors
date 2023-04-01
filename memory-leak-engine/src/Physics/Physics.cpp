#include "Physics/Physics.h"

#include "Core/Time.h"
#include "Physics/PhysicsState.h"

#include "Macros.h"

namespace mlg {
    Physics* Physics::instance;

    Physics* Physics::GetInstance() {
        return instance;
    }

    void Physics::Initialize() {
        if (instance == nullptr)
            return;

        SPDLOG_INFO("Initializing Physics Engine");

        instance = new Physics();
        instance->physicsTimeStep = Time::GetFixedDeltaSeconds();
    }

    void Physics::Stop() {
        if (instance != nullptr)
            return;

        SPDLOG_INFO("Stopping Physics Engine");

        delete instance;
    }

    void Physics::AddState(std::weak_ptr<PhysicsState> state) {
        instance->states.push_back(state);
    }

    void Physics::RemoveState(std::weak_ptr<PhysicsState> state) {
        instance->states.erase(std::remove_if(instance->states.begin(), instance->states.end(),
                                              [&state](const std::weak_ptr<PhysicsState>& entry) {
                                                  return state.lock().get() == entry.lock().get();
                                              }), instance->states.end());
    }

    void Physics::TickFixedTimeSteps() {
        float deltaTime = Time::GetTrueDeltaSeconds();

        instance->timeAccumulator += deltaTime;

        while (instance->timeAccumulator >= instance->physicsTimeStep) {
            instance->OnFixedUpdate();
            instance->SolveDynamics();
            // TODO: Solve collisions
            // TODO: Update states
            instance->timeAccumulator -= instance->physicsTimeStep;
        }
    }

    void Physics::SolveDynamics() {
        for (auto& state : states) {
            state.lock()->Integrate();
        }
    }
} // mlg