#ifndef LOGGING_MACROS_H
#define LOGGING_MACROS_H

#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#endif

#include "spdlog/spdlog.h"

namespace LoggingMacros
{
    void InitializeSPDLog();
}

#endif