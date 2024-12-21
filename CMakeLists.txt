cmake_minimum_required(VERSION 3.5)

# Settings
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_PARALLEL_LEVEL 12)

set(ProjectName GBP)
project(${ProjectName})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)


# Sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include 
include_directories(src)

# GLFW
set(BUILD_SHARED_LIBS OFF)           
set(GLFW_VULKAN_STATIC ON)
set(GLFW_LIBRARY_TYPE STATIC)        
set(GLFW_BUILD_EXAMPLES OFF)         
set(GLFW_BUILD_TESTS OFF)            
set(GLFW_BUILD_DOCS OFF)             

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)
set_target_properties(glfw PROPERTIES POSITION_INDEPENDENT_CODE ON)

# GLM
include_directories(lib/GLM)

# SPDLOG
add_subdirectory(lib/spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
include_directories(lib/spdlog/include)

# Compile source files
add_executable(${ProjectName} ${SOURCES})

target_link_libraries(${ProjectName} glfw spdlog)
