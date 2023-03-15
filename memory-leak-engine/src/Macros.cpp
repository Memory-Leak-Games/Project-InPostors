#include "Macros.h"

#include "iostream"

void LoggingMacros::InitializeSPDLog() {
#ifdef DEBUG
    std::cout << "DEBUG BUILD: " << __DATE__ << " - " << __TIME__ << std::endl
              << std::endl;
    spdlog::set_level(spdlog::level::debug);
#else
    std::cout << "RELEASE BUILD: " << __DATE__ << " - " << __TIME__ << std::endl
              << std::endl;
    spdlog::set_level(spdlog::level::err);
#endif
}
