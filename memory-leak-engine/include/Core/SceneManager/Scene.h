#pragma once

namespace mlg {

    class Scene {
    public:
        virtual ~Scene() = default;

        virtual void Load() = 0;
        virtual void Start(){};
        virtual void Update(){};
        virtual void UnLoad(){};
    };

}// namespace mlg