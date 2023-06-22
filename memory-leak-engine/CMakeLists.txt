project(memory-leak-engine VERSION 1.0)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${glad_SOURCE_DIR}
        ${stb_image_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/memory-leak-engine/include)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} stb_image)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} spdlog)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} effolkronium_random)
target_link_libraries(${PROJECT_NAME} eventpp)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} magic_enum)
target_link_libraries(${PROJECT_NAME} freetype)
target_link_libraries(${PROJECT_NAME} soloud)
target_link_libraries(${PROJECT_NAME} magic_enum)
target_link_libraries(${PROJECT_NAME} tinyobjloader)
target_link_libraries(${PROJECT_NAME} Tracy::TracyClient)
target_link_libraries(${PROJECT_NAME} SDL2-static)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} -static-libstdc++)
else()
    target_link_libraries(${PROJECT_NAME} stdc++)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} imgui)
    target_link_libraries(${PROJECT_NAME} imgui_impl)

    target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})

    if (WIN32)
        add_definitions(-D_WIN32_WINNT=0x0602 -DWINVER=0x0602)
        target_link_libraries(${PROJECT_NAME} ws2_32 wsock32 -ldbghelp)
    endif ()
endif ()

