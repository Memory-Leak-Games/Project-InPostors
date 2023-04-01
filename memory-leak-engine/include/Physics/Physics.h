#pragma once

namespace mlg {

    class Physics {
    private:
        static Physics* instance;

        float physicsTimeStep;
        float timeAccumulator = 0;

        std::vector<std::weak_ptr<class PhysicsState>> states;

        eventpp::CallbackList<void ()> OnFixedUpdate;

        Physics() = default;
    public:
        static void Initialize();
        static void Stop();

        static Physics* GetInstance();

        static void AddState(std::weak_ptr<PhysicsState> state);
        static void RemoveState(std::weak_ptr<PhysicsState> state);

        static void TickFixedTimeSteps();

        void SolveDynamics();
    };

} // mlg
