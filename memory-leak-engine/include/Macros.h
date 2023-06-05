#pragma once

#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#endif

#include "spdlog/spdlog.h"

namespace LoggingMacros {
    void InitializeSPDLog();
}

// Assertions

#ifdef DEBUG
#define MLG_ASSERT_MSG(condition, message)                                 \
    do {                                                                   \
        if (!(condition)) {                                                \
            SPDLOG_ERROR("Assertion `" #condition "` failed {}", message); \
            std::terminate();                                              \
        }                                                                  \
    } while (false)

#define MLG_ASSERT(condition)                                  \
    do {                                                       \
        if (!(condition)) {                                    \
            SPDLOG_ERROR("Assertion `" #condition "` failed"); \
            std::terminate();                                  \
        }                                                      \
    } while (false)

#define MLG_UNIMPLEMENTED                        \
    do {                                         \
        SPDLOG_ERROR("Unimplemented Assertion"); \
        std::terminate();                        \
    } while (false)

#else
#define MLG_ASSERT_MSG(condition, message) \
    do {                                   \
    } while (false)
#define MLG_ASSERT(condition) \
    do {                      \
    } while (false)
#define MLG_UNIMPLEMENTED \
    do {                  \
    } while (false)
#endif

#ifndef DEBUG
#undef TRACY_ENABLE
#endif

#define TRACY_ON_DEMAND

#include "common/TracyColor.hpp"
#include "glad/glad.h"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

// Get x bit
#define MLG_BIT(x) (1 << x)
#define MLG_READBIT(x, n) ((x >> n) & 1)
#define MLG_SETBIT(x, n) (x |= (1 << n))
#define MLG_UNSETBIT(x, n) (x &= ~(1 << n))
#define MLG_TOGGLEBIT(x, n) (x ^= 1 << n)

#define MLG_READFLAG(x, flag) ((x & flag) == flag)

#define MLG_READMASK(x, mask) (x & mask)
#define MLG_SETMASK(x, mask) x |= mask
#define MLG_UNSETMASK(x, mask) x &= ~mask
#define MLG_TOGGLEMASK(x, mask) x ^= mask
