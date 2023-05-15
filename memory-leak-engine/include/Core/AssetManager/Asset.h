#pragma once

#include <memory>
#include <filesystem>
#include <utility>

namespace mlg {

    class Asset {
        std::string path;
    public:
        explicit Asset(std::string path) : path(std::move(path)) {
            if (!exists(std::filesystem::path(this->path))) {
                SPDLOG_ERROR("Path: {} does not exist.", this->path);
                MLG_ASSERT(exists(std::filesystem::path(this->path)));
            }
        };

        virtual void Load() = 0;
        virtual ~Asset() = default;

        [[nodiscard]] const std::string& GetPath() const {
            return path;
        }
    };

}// namespace mlg
