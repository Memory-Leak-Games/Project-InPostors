#pragma once

#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#endif

#include "spdlog/spdlog.h"

#include "glad/glad.h"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace LoggingMacros {
    void InitializeSPDLog();
}

// Assertions

#ifdef DEBUG
#define MLG_ASSERT_MSG(condition, message)                                     \
    do {                                                                   \
        if (!(condition)) {                                                \
            SPDLOG_ERROR("Assertion `" #condition "` failed {}", message); \
            std::terminate();                                              \
        }                                                                  \
    } while (false)

#define MLG_ASSERT(condition)                                     \
    do {                                                                   \
        if (!(condition)) {                                                \
            SPDLOG_ERROR("Assertion `" #condition "` failed"); \
            std::terminate();                                              \
        }                                                                  \
    } while (false)

#define MLG_UNIMPLEMENTED                      \
    do {                                         \
        SPDLOG_ERROR("Unimplemented Assertion"); \
        std::terminate();                        \
    } while (false)

#else
#define MLG_ASSERT_MSG(condition, message) \
    do {                               \
        condition;                     \
    } while (false)
#define MLG_ASSERT(condition) \
    do {                               \
        condition;                     \
    } while (false)
#define MLG_UNIMPLEMENTED \
    do {                  \
    } while (false)
#endif

// Get x bit
#define MLG_BIT(x) (1 << x)