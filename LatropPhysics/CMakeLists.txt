cmake_minimum_required (VERSION 3.20)
project (LatropPhysics C CXX)

file(GLOB HEADER_FILES Sources/*.h)
file(GLOB SOURCE_FILES Sources/*.cpp)

# Set the GLM path based on the VULKAN_SDK environment variable
if(DEFINED ENV{VULKAN_SDK})
    set(GLM_PATH $ENV{VULKAN_SDK}/include)
    include_directories(${GLM_PATH})
else()
    message(FATAL_ERROR "VULKAN_SDK environment variable not set")
endif()

include_directories(${PROJECT_SOURCE_DIR} ${GLM_INCLUDE_DIRS})

add_library(LatropPhysics STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_precompile_headers(LatropPhysics PUBLIC ${HEADER_FILES})
target_compile_features(LatropPhysics PRIVATE cxx_std_17)