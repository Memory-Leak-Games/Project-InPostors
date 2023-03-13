#include "Macros.h"

void LoggingMacros::InitializeSPDLog()
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::err);
#endif
}
