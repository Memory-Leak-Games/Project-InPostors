project(memory-leak-engine VERSION 1.0)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
        src/*.cpp
        imgui_impl/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        include/*.h
        imgui_impl/*.h)

# Define the executable
add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${glad_SOURCE_DIR}
        ${stb_image_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/memory-leak-engine/include)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} stb_image)
target_link_libraries(${PROJECT_NAME} assimp)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} spdlog)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} effolkronium_random)
target_link_libraries(${PROJECT_NAME} eventpp)
