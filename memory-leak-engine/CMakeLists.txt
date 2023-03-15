project(memory-leak-engine VERSION 1.0)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    file(GLOB_RECURSE IMGUI_SOURCE_FILES imgui_impl/*.cpp)
    file(GLOB_RECURSE IMGUI_HEADER_FILES include/*.h)

    add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_SOURCE_FILES})
else()
    add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_HEADER_FILES})
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${glad_SOURCE_DIR}
        ${stb_image_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/memory-leak-engine/include)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} stb_image)
target_link_libraries(${PROJECT_NAME} assimp)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} spdlog)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} effolkronium_random)
target_link_libraries(${PROJECT_NAME} eventpp)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} magic_enum)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} imgui)
    target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
endif()
