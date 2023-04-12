# Add source files
file(GLOB_RECURSE SOURCE_FILES
        src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        include/*.h)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/project-inpostors/include
        ${CMAKE_SOURCE_DIR}/memory-leak-engine/include)

target_link_libraries(${PROJECT_NAME} memory-leak-engine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res)
