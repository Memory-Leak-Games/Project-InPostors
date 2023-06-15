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

set(GLFW_USE_WAYLAND ON)

if (EXISTS "/usr/bin/mold")
    add_link_options("-fuse-ld=mold")
endif()


