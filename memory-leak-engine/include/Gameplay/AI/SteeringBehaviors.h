#pragma once

namespace mlg {

    class SteeringBehaviors {
    public:
        enum SummingMethod {
            weightedAverage,
            prioritized,
            dithered
        };

    private:
        enum behaviorType {
            none               = 0x00000,
            seek               = 0x00002,
            flee               = 0x00004,
            arrive             = 0x00008,
            wander             = 0x00010,
            cohesion           = 0x00020,
            separation         = 0x00040,
            alignment          = 0x00080,
            obstacleAvoidance  = 0x00100,
            wallAvoidance      = 0x00200,
            followPath         = 0x00400,
            pursuit            = 0x00800,
            evade              = 0x01000,
            interpose          = 0x02000,
            hide               = 0x04000,
            flock              = 0x08000,
            offset_pursuit     = 0x10000,
        };

        uint64_t flags;

        enum Deceleration {
            slow = 3,
            normal = 2,
            fast = 1
        };

        Deceleration deceleration;
        SummingMethod summingMethod;

        glm::vec2 steeringForce;
        glm::vec2 target;

        // Weights for adjusting behavior strength
        float cohesionWeight;
        float separationWeight;
        float alignmentWeight;
        float seekWeight;
        float arriveWeight;
        float followPathWeight;

        bool BehaviorTypeOn(behaviorType bt) { return (flags & bt) == bt; }
        bool AccumulateForce(glm::vec2& sf, glm::vec2 forceToAdd);

        // Behavior declarations
        glm::vec2 Seek(glm::vec2 targetPos);
        glm::vec2 Arrive(glm::vec2 targetPos, Deceleration deceleration);
        glm::vec2 FollowPath();

        // Group behaviors
        template <typename T>
        glm::vec2 Cohesion(const std::vector<std::weak_ptr<T>> &agents);

        template <typename T>
        glm::vec2 Separation(const std::vector<std::weak_ptr<T>> &agents);

        template <typename T>
        glm::vec2 Alignment(const std::vector<std::weak_ptr<T>> &agents);

        // Different calculation methods
        glm::vec2 CalculateWeightedSum();
        glm::vec2 CalculatePrioritized();
        glm::vec2 CalculateDithered();

    public:
        SteeringBehaviors();
        virtual ~SteeringBehaviors();

        glm::vec2 Calculate(float viewDistance);

        glm::vec2 GetSteeringForce() const { return steeringForce; }

        void SetBehaviorWeights(float cohesion, float separation, float alignment,
                                float seek, float arrive, float follow);

        void SetSummingMethod(SummingMethod sm) { summingMethod = sm; }

        // Flag checks
        void SeekOn() { flags |= seek; }
        void ArriveOn() { flags |= arrive; }
        void CohesionOn() { flags |= cohesion; }
        void SeparationOn() { flags |= separation; }
        void AlignmentOn() { flags |= alignment; }
        void FollowPathOn() { flags |= followPath; }

        void SeekOff() { if(BehaviorTypeOn(seek)) flags ^= seek; }
        void ArriveOff() { if(BehaviorTypeOn(arrive)) flags ^= arrive; }
        void CohesionOff() { if(BehaviorTypeOn(cohesion)) flags ^= cohesion; }
        void SeparationOff() { if(BehaviorTypeOn(separation)) flags ^= separation; }
        void AlignmentOff() { if(BehaviorTypeOn(alignment)) flags ^= alignment; }
        void FollowPathOff() { if(BehaviorTypeOn(followPath)) flags ^= followPath; }

        bool isSeekOn() { return BehaviorTypeOn(seek); }
        bool isArriveOn() { return BehaviorTypeOn(arrive); }
        bool isCohesionOn() { return BehaviorTypeOn(cohesion); }
        bool isSeparationOn() { return BehaviorTypeOn(separation); }
        bool isAlignmentOn() { return BehaviorTypeOn(alignment); }
        bool isFollowPathOn() { return BehaviorTypeOn(followPath); }

        float GetSeparationWeight() const { return separationWeight; }
        float GetAlignmentWeight() const { return alignmentWeight; }
        float GetCohesionWeight() const { return cohesionWeight; }
    };

}// namespace mlg
