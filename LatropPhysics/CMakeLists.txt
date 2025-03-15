cmake_minimum_required(VERSION 3.20)

# MARK: - Library
# Project name and languages
project(LatropPhysics C CXX)

# File globs for headers and sources
file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

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

# Add the library for LatropPhysics
add_library(LatropPhysics STATIC ${SOURCE_FILES} ${HEADER_FILES})

# Include directories for the library
target_include_directories(LatropPhysics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
include_directories(${GLM_PATH})

# Set C++ standard for the library
target_compile_features(LatropPhysics PRIVATE cxx_std_17)

# MARK: - Testing
# Option to enable or disable tests
option(ENABLE_TESTING "Enable building tests" ON)

if(ENABLE_TESTING)
    # Find Google Test package
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    # Add the test executable
    file(GLOB_RECURSE TEST_SOURCE_FILES tests/*.cpp)
    add_executable(LatropPhysicsTests ${TEST_SOURCE_FILES})

    # Link the test executable with the LatropPhysics library and Google Test
    target_link_libraries(LatropPhysicsTests
        LatropPhysics      # Link your LatropPhysics library
        ${GTEST_LIBRARIES}  # Link Google Test
        pthread             # For POSIX systems (Linux/macOS)
    )

    # Optionally: Enable testing and automatically run the tests when building
    enable_testing()
    add_test(NAME LatropPhysicsTests COMMAND LatropPhysicsTests)

    gtest_discover_tests(LatropPhysicsTests)
endif()