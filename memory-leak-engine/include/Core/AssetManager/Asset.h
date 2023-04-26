#pragma once

#include <memory>
#include <filesystem>

namespace mlg {

    class Asset {
        std::string path;
    public:
        explicit Asset(std::string path) : path(std::move(path)) {
            MLG_ASSERT(exists(std::filesystem::path(this->path)));
        };

        virtual void Load() = 0;
        virtual ~Asset() = default;

        [[nodiscard]] const std::string& GetPath() const {
            return path;
        }
    };

}// namespace mlg
