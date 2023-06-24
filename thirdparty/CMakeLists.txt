# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PRIVATE ${STB_IMAGE_DIR})

# SoLoud
INCLUDE(SoLoud.cmake)

# SDL
INCLUDE(SDL.cmake)

target_include_directories(soloud PUBLIC ${soloud_SOURCE_DIR}/include)
set_target_properties(soloud PROPERTIES FOLDER thirdparty)

# Other
CPMAddPackage(
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.8
    OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_EXAMPLES OFF"
)

CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.1.1
  OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
)

CPMAddPackage(
    GITHUB_REPOSITORY wolfpld/tracy
    VERSION 0.9.1
    OPTIONS "TRACY_ENABLE ON"
            "TRACY_ON_DEMAND ON"
            "TRACY_NO_BROADCAST ON"
            "TRACY_NO_SYSTEM_TRACING ON"
            "TRACY_TIMER_QPC ON"
)

CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:effolkronium/random@1.4.1")
CPMAddPackage("gh:wqking/eventpp@0.1.2")
CPMAddPackage("gh:nlohmann/json@3.11.2")
CPMAddPackage("gh:Neargye/magic_enum@0.8.2")
CPMAddPackage("gh:freetype/freetype#VER-2-13-0")
CPMAddPackage("gh:tinyobjloader/tinyobjloader@2.0.0rc10")

set_target_properties(glad
        stb_image
        glfw
        glm
        eventpp
        nlohmann_json
        magic_enum
        freetype
        magic_enum
        tinyobjloader
        spdlog PROPERTIES FOLDER "thirdparty")

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif ()

# imGUI
if (CMAKE_BUILD_TYPE MATCHES Debug)
    CPMAddPackage("gh:ocornut/imgui@1.88")

    add_library(imgui STATIC
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp # optionally comment this out
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp

            )

    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

    add_library(imgui_impl STATIC
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            )
    target_link_libraries(imgui_impl imgui)
    target_link_libraries(imgui_impl glfw)

    target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR}/backends)

    target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)
    set_target_properties(imgui PROPERTIES FOLDER thirdparty)
endif ()