#pragma once

namespace mlg {

    class Asset {
        std::string path;
    public:
        Asset(std::string path)
                : path(path) {};

        virtual ~Asset() = 0;
    };

}// namespace mlg
