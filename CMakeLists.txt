cmake_minimum_required (VERSION 3.20)
     
project (LatropGame CXX)

include(FetchContent)

set(CMAKE_SKIP_INSTALL_RULES ON)
set(CMAKE_SUPPRESS_REGENERATION ON)

file(GLOB SOURCE_FILES src/*.h src/*.cpp)

find_package(Vulkan REQUIRED)

add_subdirectory(LizardGraphics-2.0)
add_subdirectory(LatropPhysics)

add_executable(LatropGame ${SOURCE_FILES})

target_compile_features(LatropGame PRIVATE cxx_std_20)
target_link_libraries(LatropGame LizardGraphics ${Vulkan_LIBRARIES} glfw LatropPhysics TracyClient)