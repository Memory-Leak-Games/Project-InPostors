#pragma once

#include "Gameplay/Component.h"

namespace mlg {
    class Entity;
}

class StorageUI : public mlg::Component {
public:
    StorageUI(const std::weak_ptr<class mlg::Entity>& owner, const std::string& name);

};
