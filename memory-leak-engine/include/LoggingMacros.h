#ifndef LOGGING_MACROS_H
#define LOGGING_MACROS_H

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
#   define MLG_ASSERT(condition, message) \
    do { \
        if (! (condition)) {          \
            SPDLOG_ERROR("Assertion `" #condition "` failed {}", message);\
            std::terminate(); \
        } \
    } while (false)

# define MLG_UNIMPLEMENTED() \
    do { \
        SPDLOG_ERROR("Unimplemented Assertion");\
        std::terminate(); \
    } while (false)

#else
#   define MLG_ASSERT(condition, message) do { } while (false)
#   define MLG_UNIMPLEMENTED do {} while (false)
#endif

#endif