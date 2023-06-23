include(CMakePrintHelpers)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

CMAKE_POLICY(SET CMP0012 NEW)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

set(GLFW_USE_WAYLAND OFF)

if (EXISTS "/usr/bin/mold" AND UNIX)
    add_link_options("-fuse-ld=mold")
endif()

set(BUILD_SHARED_LIBS OFF)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()