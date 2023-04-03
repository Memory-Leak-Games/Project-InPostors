#pragma once

#include <memory>

namespace mlg {

    class Asset {
    protected:
        std::string path;
    public:
        explicit Asset(std::string path) : path(std::move(path)) {};

        virtual void Load() = 0;
        virtual ~Asset() = default;

        [[nodiscard]] const std::string& GetPath() const {
            return path;
        }
    };

}// namespace mlg
