cmake_minimum_required (VERSION 3.20)
project (LatropPhysics C CXX)

file(GLOB HEADER_FILES include/*.h)
file(GLOB SOURCE_FILES src/*.cpp)

# Set the GLM path based on the VULKAN_SDK environment variable
if(DEFINED ENV{VULKAN_SDK})
    set(GLM_PATH $ENV{VULKAN_SDK}/include)
    include_directories(${GLM_PATH})
else()
    message(STATUS "VULKAN_SDK not found, searching for GLM...")
    find_package(GLM REQUIRED)
    include_directories(${GLM_INCLUDE_DIRS})
endif()

add_library(LatropPhysics STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(LatropPhysics PUBLIC ${CMAKE_SOURCE_DIR}/include)
include_directories(${GLM_PATH})

target_compile_features(LatropPhysics PRIVATE cxx_std_17)