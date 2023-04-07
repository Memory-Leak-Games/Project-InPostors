#pragma once

#include "Collider.h"

namespace mlg {

    class CircleCollider : public Collider {
    private:
        float radius;

    public:
        explicit CircleCollider(float radius);

        ColliderType GetColliderType() override;
        float GetRadius() override;
    };

}// namespace mlg
