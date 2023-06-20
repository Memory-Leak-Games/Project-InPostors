#include "Animation/FerrisWheelAnim.h"

FerrisWheelAnim::FerrisWheelAnim(
        uint64_t id,
        const std::string& name,
        bool isStatic,
        mlg::Transform* parent)
    : mlg::Entity(id, name, isStatic, parent) {

}

std::shared_ptr<FerrisWheelAnim> FerrisWheelAnim::Create(uint64_t id, const std::string& name,
                                               bool isStatic, mlg::Transform* parent) {
    auto newFerrisWheel = std::shared_ptr<FerrisWheelAnim>(
            new FerrisWheelAnim(id, name, isStatic, parent));

    std::ifstream configFile()
}

void FerrisWheelAnim::Start() {

}

void FerrisWheelAnim::Update() {

}
