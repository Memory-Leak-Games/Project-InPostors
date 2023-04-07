#pragma once

namespace mlg {

    class Physics {
    public:
        eventpp::CallbackList<void ()> OnFixedUpdate;

    private:
        static Physics* instance;

        float timeAccumulator = 0;

        std::vector<std::weak_ptr<class Rigidbody>> states;

        Physics() = default;
    public:
        static void Initialize();
        static void Stop();

        static Physics* GetInstance();

        static void AddRigidbody(std::weak_ptr<Rigidbody> state);
        static void RemoveRigidbody(std::weak_ptr<Rigidbody> state);

        static void TickFixedTimeSteps();

        void SolveDynamics();
    };

} // mlg
