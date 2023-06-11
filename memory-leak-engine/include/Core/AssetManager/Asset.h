#pragma once

#include "Macros.h"
#include <cctype>
#include <filesystem>
#include <memory>
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

#ifdef DEBUG
            if (!IsSnakeCase(this->path)) {
                SPDLOG_WARN(
                        "Path: {} is not a snake case. You just killed a kitty. How dare you!",
                        this->path);
                MLG_ASSERT(IsSnakeCase(this->path));
            }
#endif
        };

        virtual void Load() = 0;
        virtual ~Asset() = default;

        [[nodiscard]] const std::string& GetPath() const {
            return path;
        }

    private:
        static bool IsSnakeCase(const std::string& str) {
            for (auto c : str) {
                if (c == '_' || c == '.' || c == '/' || c == '-')
                    continue;
                if (std::isupper(c))
                    return false;
                if (!std::isalnum(c, std::locale::classic()))
                    return false;
            }
            return true;
        }
    };

}// namespace mlg
