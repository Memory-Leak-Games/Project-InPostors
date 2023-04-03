#include "Physics/Physics.h"

#include "Core/Time.h"
#include "Physics/Rigidbody.h"

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
    }

    void Physics::Stop() {
        if (instance == nullptr)
            return;

        SPDLOG_INFO("Stopping Physics Engine");

        delete instance;
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
        float deltaTime = Time::GetTrueDeltaSeconds();

        instance->timeAccumulator += deltaTime;

        int test = 0;
        while (instance->timeAccumulator >= Time::GetFixedTimeStep()) {
            instance->OnFixedUpdate();
            instance->SolveDynamics();
            // TODO: Solve collisions
            // TODO: Update states

            instance->timeAccumulator -= Time::GetFixedTimeStep();
            test++;
        }
    }

    void Physics::SolveDynamics() {
        for (auto& state : states) {
            if (state.expired())
                continue;

            state.lock()->Integrate();
        }
    }
} // mlg