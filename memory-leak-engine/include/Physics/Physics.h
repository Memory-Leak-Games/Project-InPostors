#pragma once

namespace mlg {

    class Physics {
    public:
        eventpp::CallbackList<void ()> OnFixedUpdate;

    private:
        static Physics* instance;

        float timeAccumulator = 0;

        std::vector<std::weak_ptr<class PhysicsState>> states;

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
