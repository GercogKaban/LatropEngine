# MARK: Project Definition
cmake_minimum_required(VERSION 3.20)
project(LatropEngine C CXX)

# Set C++ standard for all targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable a more sophisticated compiler(if available)
find_program(CLANG_C_COMPILER clang)
find_program(CLANG_CXX_COMPILER clang++)

if (CLANG_C_COMPILER AND CLANG_CXX_COMPILER)
    message("Found clang")
    set(CMAKE_C_COMPILER ${CLANG_C_COMPILER} CACHE STRING "C Compiler" FORCE)
    set(CMAKE_CXX_COMPILER ${CLANG_CXX_COMPILER} CACHE STRING "C++ Compiler" FORCE)
endif()

# Select target-specific python name
if(WIN32)
    set(PYTHON_COMMAND python)
else()
    set(PYTHON_COMMAND python3)
endif()

# MARK: - Dependencies
# MARK: Vulkan
find_package(Vulkan REQUIRED)

# MARK: GLM(kinda)
# Set the GLM path based on the VULKAN_SDK environment variable or located
# using the find_package command
if(DEFINED ENV{VULKAN_SDK})
    set(GLM_PATH $ENV{VULKAN_SDK}/include)
    include_directories(${GLM_PATH})
else()
    message(STATUS "VULKAN_SDK not found, searching for GLM...")
    find_package(GLM REQUIRED)
    include_directories(${GLM_INCLUDE_DIRS})
endif()

# MARK: GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(dependencies/glfw)

# MARK: Tracy Profiler
add_subdirectory(dependencies/tracy-profiler)
add_compile_definitions(TRACY_ENABLE)
add_compile_definitions(TRACY_ON_DEMAND)

include(FetchContent)

# MARK: - Modules
# MARK: Physics

# File globs for headers and sources
file(GLOB_RECURSE LP_HEADER_FILES sources/physics/include/*.h)
file(GLOB_RECURSE LP_SOURCE_FILES sources/physics/src/*.cpp)

# Add the library for Physics module
add_library(LatropPhysics STATIC ${LP_SOURCE_FILES} ${LP_HEADER_FILES})

# Include directories for the library
target_include_directories(LatropPhysics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/physics/include>
    $<INSTALL_INTERFACE:sources/physics/include>
)
include_directories(${GLM_PATH})

# MARK: Graphics
set(CMAKE_SKIP_INSTALL_RULES ON)
set(CMAKE_SUPPRESS_REGENERATION ON)

file(GLOB_RECURSE LG_HEADER_FILES sources/graphics/include/*.h)
# TODO: Move shaders someplace else
file(GLOB_RECURSE LG_SOURCE_FILES sources/graphics/src/*.cpp sources/graphics/src/*.frag sources/graphics/src/*.vert)

add_library(LatropGraphics STATIC ${LG_SOURCE_FILES} ${LG_HEADER_FILES})
target_include_directories(LatropGraphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/graphics/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include/GLFW>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/magic_enum/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/VulkanMemoryAllocator/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tracy-profiler/public>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies>
    $<INSTALL_INTERFACE:sources/graphics/include>
    ${Vulkan_INCLUDE_DIR}
)

add_custom_target(run_script ALL 
    COMMAND ${CMAKE_COMMAND} -E env PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}/sources/graphics ${PYTHON_COMMAND} ${PROJECT_SOURCE_DIR}/scripts/build_shaders.py
    COMMENT "Running script before build"
)

add_dependencies(LatropGraphics run_script)

target_precompile_headers(LatropGraphics PUBLIC sources/graphics/include/pch.h)

# MARK: Engine
# File globs for headers and sources
file(GLOB_RECURSE LE_HEADER_FILES sources/engine/include/*.h)
file(GLOB_RECURSE LE_SOURCE_FILES sources/engine/src/*.cpp)

add_library(LatropEngine STATIC ${LE_SOURCE_FILES} ${LE_HEADER_FILES})

# Include directories for the engine
target_include_directories(LatropEngine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/engine/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/physics/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/graphics/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tracy-profiler/public>
    $<INSTALL_INTERFACE:sources/engine/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include/GLFW>
)

if(WIN32)
set_target_properties(LatropEngine PROPERTIES VS_PLATFORM_TOOLSET ClangCL)
endif()

# MARK: Game
file(GLOB_RECURSE L_HEADER_FILES sources/game/include/*.h)
file(GLOB_RECURSE L_SOURCE_FILES sources/game/src/*.cpp)

add_executable(LatropGame ${L_SOURCE_FILES} ${L_HEADER_FILES})

target_include_directories(LatropGame PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/engine/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/physics/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/graphics/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources/game/include>
    $<INSTALL_INTERFACE:sources/game/include>
)

add_custom_target(move_textures ALL 
    COMMAND ${CMAKE_COMMAND} -E env PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR} ${PYTHON_COMMAND} ${PROJECT_SOURCE_DIR}/scripts/move_textures.py ${PROJECT_SOURCE_DIR}/sources/graphics ${CMAKE_BINARY_DIR}
    COMMENT "Running script before build"
)

add_dependencies(LatropGame move_textures)

target_link_libraries(LatropGame LatropEngine LatropGraphics ${Vulkan_LIBRARIES} glfw LatropPhysics TracyClient)