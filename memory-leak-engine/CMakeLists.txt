project(memory-leak-engine VERSION 1.0)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    file(GLOB_RECURSE IMGUI_SOURCE_FILES imgui_impl/*.cpp)
    file(GLOB_RECURSE IMGUI_HEADER_FILES include/*.h)

    add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_SOURCE_FILES})
else()
    add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${glad_SOURCE_DIR}
        ${stb_image_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/memory-leak-engine/include)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC stb_image)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC effolkronium_random)
target_link_libraries(${PROJECT_NAME} PUBLIC eventpp)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PUBLIC magic_enum)
target_link_libraries(${PROJECT_NAME} PUBLIC freetype)

find_package(OpenAL)
if (${OPENAL_FOUND})
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenAL::OpenAL)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenAL)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
    target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
endif()
