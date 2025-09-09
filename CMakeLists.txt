cmake_minimum_required(VERSION 3.26)
project(RayTracer)

include(FetchContent)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif(UNIX)

# Add executable
add_executable(${PROJECT_NAME})

# IMGL
add_subdirectory("IMGL")
target_include_directories(${PROJECT_NAME} PRIVATE "IMGL/src")

# Bring in shaders
add_custom_command(
    TARGET RayTracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/IMGL/shaders/"
            "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    COMMENT "Copying in shaders"
)

# Bring in fonts
add_custom_command(
    TARGET RayTracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "IMGL/fonts/"
            "${CMAKE_CURRENT_BINARY_DIR}/fonts"
    COMMENT "Copying in fonts"
)

# Headers
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Sources
add_subdirectory("src")

# Bring in custom shaders
add_custom_command(
    TARGET RayTracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
            "${CMAKE_CURRENT_BINARY_DIR}/app_shaders"
    COMMENT "Copying in shaders"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE IMGL)