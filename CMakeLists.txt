cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(InPostors VERSION 1.0)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

message(${CMAKE_BUILD_TYPE})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(global_settings)

# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Engine project's files ----
add_subdirectory(memory-leak-engine)

# ----
add_subdirectory(project-inpostors)
